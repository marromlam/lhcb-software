// $Id: CombineParticles.cpp,v 1.23 2008-11-06 11:52:36 pkoppenb Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/GetDecay.h"
#include "Kernel/GetParticlesForDecay.h"
#include "Kernel/IPlotTool.h"
#include "Kernel/ISetInputParticles.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
#include "LoKi/IHybridFactory.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/CompareParticles.h"
// ============================================================================
#include "LoKi/Selected.h"
#include "LoKi/Combiner.h"
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
#include "LoKi/Particles0.h"
#include "LoKi/PIDOperators.h"
// ============================================================================
// Boots 
// ============================================================================
#include "boost/algorithm/string.hpp"
// ============================================================================
namespace 
{
  /// make the plots ? 
  inline bool validPlots ( std::string name ) 
  {
    if ( name.empty() ) { return false ; }
    boost::to_lower ( name ) ;
    return "none" != name ;
  }
}
// ============================================================================
/** @class CombineParticles 
 *  LoKi-based version of CombineParticles algorithm, developed by 
 *  Gerhard Raven, Patrick Koppenburg and Juan Palacios. 
 * 
 *  The major propertes of the algorithm (in addition to the 
 *   properties of class DValgorithm) are:
 *    - <c>"Factory"</c>:   the type/name of hybrid (LoKi/Bender) C++/Python factory
 *    - <c>"Preambulo"</c>: the preambulo to be used for Bender/Python script
 *    - <c>"DecayDescriptors"</c>: the list of decay descriptors 
 *    - <c>"DaughtesCuts"</c>:     the map of cuts for daughters particles
 *    - <c>"CombinationCut"</c>:   the cut applied for combination of particles 
 *    - <c>"MotherCut"</c>:        the cut applied for mother particle 
 *    - <c>"DaughtersPlots"<c>:    the type/name of Plot-Tool for daughter particles 
 *    - <c>"CombinationPlots"<c>:  the type/name of Plot-Tool for combination  
 *    - <c>"MotherPlots"<c>:       the type/name of Plot-Tool for mother particles 
 *    - <c>"DaughtersPlotsPath"<c>:    the path for Daughters Plots 
 *    - <c>"CombinationPlotsPath"<c>:  the path for Combination Plots 
 *    - <c>"MotherPlotsPath"<c>:       the path for Mother Plots 
 *  
 *  Few counters are produced:
 *    - "# <PID>" for each daughter 'PID': bumber of correspobding daugter particles 
 *    - "# <DECAY>" for each <decay>: number of selected decay candidates 
 *    - "# selected": total number of selected candidates 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-01
 */
class CombineParticles 
  : public         DVAlgorithm 
  , public virtual ISetInputParticles 
  , public virtual IIncidentListener 
{
  // ==========================================================================
  // the friend factory, needed fo rinstantiation
  friend class AlgFactory<CombineParticles> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the standard initialization of the algorithm 
  virtual StatusCode initialize () ; // standard initialization 
  /// the standard execution      of the algorithm 
  virtual StatusCode execute    () ; // standard execution 
  // ==========================================================================
public:
  // ==========================================================================
  // IIncindentListener interface 
  // ==========================================================================
  /** Inform that a new incident has occured
   *  @see IIncidentListener 
   */
  virtual void handle ( const Incident& ) ;
  // ==========================================================================
public:
  // ==========================================================================
  // IInterface
  // ==========================================================================
  /** Query interfaces of Interface
   *  @see IInterface 
   *  @param iid ID of Interface to be retrieved
   *  @param iif Pointer to Location for interface pointer
   */
  virtual StatusCode queryInterface 
  ( const InterfaceID& iid ,
    void**             iif ) ;
  // ==========================================================================
public:
  // ==========================================================================
  // ISetInputParticles interface 
  // ==========================================================================
  /** set the input particles
   *  @see ISetInputParticles 
   *  @param input the vector of input particles 
   *  @return status code 
   */
  virtual StatusCode setInput 
  ( const LHCb::Particle::ConstVector& input ) ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** the standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc the service locator 
   */
  CombineParticles 
  ( const std::string& name ,   // the algorithm instance name 
    ISvcLocator*       pSvc ) ; // the service locator
  /// virtual and protected destrcutor 
  virtual ~CombineParticles() {} // virtual and protected destrcutor
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  CombineParticles () ;                            // no default constructor!
  /// the copy constructor is disabled 
  CombineParticles ( const CombineParticles& ) ;      // no copy constructor!
  /// the assignement is disabled 
  CombineParticles& operator=( const CombineParticles& ) ; // no assignement!
  // ==========================================================================
private:
  // ==========================================================================
  /// decode all cuts 
  StatusCode decodeAllCuts ()  ;
  // ==========================================================================
  /// construct the preambulo string 
  std::string preambulo() const 
  {
    std::string result ;
    for ( std::vector<std::string>::const_iterator iline = 
            m_preambulo.begin() ; m_preambulo.end() != iline ; ++iline ) 
    {
      if ( m_preambulo.begin() != iline ) { result += "\n" ; }
      result += (*iline) ;
    }
    return result ;
  }
  // ==========================================================================
  /// perform the update of the major properties 
  StatusCode updateMajor  () ;
  /// perform the update of histogram properties 
  StatusCode updateHistos () ;
  /// helper to get histo tools
  StatusCode getHistoTool( IPlotTool*& histoTool, std::string name, std::string path);
  // ==========================================================================  
public:
  // ==========================================================================
  /// the handle for the property update
  void propertyHandler1 ( Property& p ) ;
  void propertyHandler2 ( Property& p ) ;
  // ==========================================================================
private:
  // ==========================================================================
  // the map of cuts for daughters { "PID" : "cuts" } 
  typedef std::map<std::string,std::string>   Map     ;
  // the actual type type for decay descriptors
  typedef std::vector<std::string>            Strings ;
  // ==========================================================================
  /** @class MyCut helpe structure to hold the cuts 
   *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
   *  @date 2008-04-19
   */
  class MyCut 
  {
  public:
    // the cinstructor 
    MyCut() 
      : m_pid  ()  // the particle PID  
      , m_name ()  // the particle NAME 
      , m_cut ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant ( true ) )
    {}
  public:
    /// the particle pid
    LHCb::ParticleID     m_pid  ; // the particle pid
    /// the particle name
    std::string          m_name ; // the particle name
    /// the cut itself 
    LoKi::PhysTypes::Cut m_cut  ; // the cut itself 
  };
  // ==========================================================================
  // the actual type for the vector of daughter cuts 
  typedef std::vector<MyCut>  MyCuts ;
  // ==========================================================================
private:   // properties 
  // ==========================================================================
  /// The decay descriptors: 
  Strings      m_decayDescriptors ; // The decay descriptors: 
  /// The map of the cuts for the daughter particles:
  Map          m_daughterCuts     ; // The map of the cuts for daughter particles:
  /// the cut to be applied for combination of good daughters 
  std::string  m_combinationCut   ; // the cut to be applied for combination of good daughters 
  /// the cut to be applied to the constructed mother particle 
  std::string  m_motherCut        ; // the cut to be applied to the constructed mother particle. Must be configured. 
  /// the type/name of hybrid C++/Python factory 
  std::string  m_factory          ; // the type/name of hybrid C++/Python factory 
  /// the preambulo 
  std::vector<std::string> m_preambulo ; // the preambulo 
  /// "DaughterPlots" tool 
  std::string  m_daughtersPlotsName   ; // the type/name of "Daughter Plots" tool 
  /// "CombinationPlots" tool 
  std::string  m_combinationPlotsName ; // the type/name of "Combination Plots" tool 
  /// "MotherPlots" tool 
  std::string  m_motherPlotsName      ; // the type/name of "Mother Plots" tool
  /// "DaughterPlots" path
  std::string  m_daughtersPlotsPath   ; // the path for "Daughter Plots" tool 
  /// "CombinationPlots" path 
  std::string  m_combinationPlotsPath ; // the path for "Combination Plots" tool 
  /// "MotherPlots" path 
  std::string  m_motherPlotsPath      ; // the path for "Mother Plots" tool
  // ==========================================================================
private:   // local stuff
  // ==========================================================================
  /// The actual list of decays 
  LHCb::Decays      m_decays   ; // The actual list of decays 
  /// the vector of daughter cuts 
  MyCuts            m_cuts     ; // the vector of daughter cuts 
  /// the actual cut for combination of good daughters 
  LoKi::Types::ACut m_acut     ; // the actual cut for combination of good daughters 
  /// the actual 'final' cut for the mother particle 
  LoKi::Types::Cut  m_cut      ; // the actual 'final' cut for the mother particle
  // ==========================================================================
  /// "Daughter    Plots" tool 
  IPlotTool*        m_daughtersPlots   ; //  the  "Daughter Plots" tool 
  /// "Combination Plots" tool 
  IPlotTool*        m_combinationPlots ; //  the  "Daughter Plots" tool 
  /// "Mother      Plots" tool 
  IPlotTool*        m_motherPlots      ; //  the  "Daughter Plots" tool      
  // ==========================================================================
private: // some very specific stuff for the special studies 
  // ==========================================================================
  /// the vector of input particles to be used instead of the standard
  LHCb::Particle::ConstVector  m_inputParticles    ;
  /// the flag which signal the usage of input particles instead of the standard
  bool                         m_useInputParticles ;
  // ==========================================================================
private: // internal
  // ==========================================================================
  /// the flag to indicate the nesessity of the self-update
  bool m_to_be_updated1 ; // the flag to indicate the nesessity of the self-update
  bool m_to_be_updated2 ; // the flag to indicate the nesessity of the self-update
  // ==========================================================================
} ;
// ============================================================================
using namespace LoKi ;
// ============================================================================
/*  the standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc the service locator 
 */
// ============================================================================
CombineParticles::CombineParticles 
( const std::string& name ,   // the algorithm instance name 
  ISvcLocator*       pSvc )   // the service locator
  : DVAlgorithm(  name , pSvc ) 
  //
  // properties
  //
  , m_decayDescriptors ()  // the decay descriptors 
  , m_daughterCuts     ()  // the cuts for daughter particles 
  , m_combinationCut   ( "ATRUE" ) // the cut for the combination of daughters
  , m_motherCut        ( "Configure me!"  ) // the final cut for the mother particle. Must be configured
  , m_factory  ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC")
  , m_preambulo () // the preambulo 
  // plots:
  , m_daughtersPlotsName   ( "LoKi::Hybrid::PlotTool/DaughtersPlots"    )
  , m_combinationPlotsName ( "LoKi::Hybrid::PlotTool/CombinationPlots"  )
  , m_motherPlotsName      ( "LoKi::Hybrid::PlotTool/MotherPlots"       )
  // plot paths:
  , m_daughtersPlotsPath   ( ""  )
  , m_combinationPlotsPath ( ""  )
  , m_motherPlotsPath      ( ""  )
  //
  // locals
  //
  /// the actual list of the decays
  , m_decays ()  // the actual list of the decays
  /// the list of cuts for daughter particles
  , m_cuts   ()  // the list of cuts for daughter particles
  // the actual cut for combination of good daughters
  , m_acut ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true  ) ) 
  // the final cut the constructed mother particle
  , m_cut  ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant     ( false ) ) 
  // plots:
  , m_daughtersPlots   ( 0 )  
  , m_combinationPlots ( 0 )  
  , m_motherPlots      ( 0 )  
  // special
  , m_inputParticles    (       ) 
  , m_useInputParticles ( false )
  // update:
  , m_to_be_updated1 ( true )
  , m_to_be_updated2 ( true )
{
  //
  //
  declareProperty
    ( "DecayDescriptors" , m_decayDescriptors , 
      "The list of (simple) decays to be recontructed/selected"   ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  {
    Property* p = Gaudi::Utils::getProperty ( this , "DecayDescriptor" ) ;
    if ( 0 != p && 0 == p->updateCallBack() ) 
    { p -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ; }
  }
  //
  declareProperty 
    ( "Factory"          , m_factory          , 
      "Type/Name for C++/Python Hybrid Factory"                   ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty 
    ( "Preambulo"        , m_preambulo        ,
      "The preambulo to be used for Bender/Python script"         ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty 
    ( "DaughtersCuts"    , m_daughterCuts     , 
      "The map of cuts, to be applied for daughter particles"     ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty 
    ( "CombinationCut"   , m_combinationCut   , 
      "The cut to be applied for combination of good daughters"   ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "MotherCut"        , m_motherCut        , 
      "The cut to be applied for the constructed mother particle. Must be configured." ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty 
    ( "DaughtersPlots"   , m_daughtersPlotsName   , 
      "The type/name for 'Daughters   Plots' tool " ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty 
    ( "CombinationPlots" , m_combinationPlotsName , 
      "The type/name for 'Combination Plots' tool " ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "MotherPlots"      , m_motherPlotsName      , 
      "The type/name for 'Mother      Plots' tool " ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty 
    ( "DaughtersPlotsPath"   , m_daughtersPlotsPath  , 
      "The path for 'Daughters   Plots'" ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "CombinationPlotsPath" , m_combinationPlotsPath , 
      "The path for 'Combination Plots'" ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty 
    ( "MotherPlotsPath"      , m_motherPlotsPath      , 
      "The path for 'Mother      Plots'" ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  setProperty ( "HistoProduce" , false ) ;
  {
    Property* p = Gaudi::Utils::getProperty ( this , "HistoProduce" ) ;
    if ( 0 != p && 0 == p->updateCallBack() ) 
    { p -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ; }
  }
  //
}
// ============================================================================
// decode all cuts 
// ============================================================================
StatusCode CombineParticles::decodeAllCuts() 
{
  m_cuts.clear() ;
  
  // locate the factory
  IHybridFactory* factory = tool<IHybridFactory> ( m_factory , this ) ;
  
  // 1) decode "daughter" cuts
  for ( Map::const_iterator ic = m_daughterCuts.begin () ; 
        m_daughterCuts.end() != ic ; ++ic )
  {
    if ( ic->first.empty() ) { continue ; }
    //
    { // check the correctness of particle name
      const ParticleProperty* _pp = LoKi::Particles::ppFromName ( ic->first ) ;
      Assert ( 0 != _pp , 
               " Invalid Particle Name is used as the key : '"  + (ic->first) + "'" ) ;
    }
    //
    MyCut item ;
    StatusCode sc = factory->get ( ic->second , item.m_cut , preambulo () ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Unable to  decode the cut for '" + ic->first + "':" + ic->second , sc ) ; }
    item.m_name = ic->first ;
    item.m_pid  = LoKi::Particles::pidFromName ( ic->first ) ;
    m_cuts.push_back ( item ) ;
    debug () << "The decoded cuts for '"+ ( ic->first ) +"' are: " << item.m_cut << endreq ;
  }  
  // 2) decode the cut for the combination of good daughters  
  {
    StatusCode sc = factory -> get ( m_combinationCut , m_acut , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode the 'A'-cut: " + m_combinationCut  , sc ) ; }  
    debug () << "The decoded cut for 'combination' is: " << m_acut << endreq ;
  }
  // 3) decode the final cut for the constructed mother particle  
  {
    StatusCode sc = factory -> get ( m_motherCut , m_cut , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'mother'-cut: " + m_motherCut  , sc ) ; }  
    debug () << "The decoded cut for 'mother' is: " << m_motherCut << endreq ;
  }
  //
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;                                        // RETURN 
}
// ============================================================================
// the standard initialization of the algorithm 
// ============================================================================
StatusCode CombineParticles::initialize ()  // standard initialization 
{
  StatusCode sc = DVAlgorithm::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  // check for LoKi service 
  svc<IService> ( "LoKiSvc" , true ) ;
  
  {
    // subscribe the incident
    IIncidentSvc* inc = svc<IIncidentSvc> ( "IncidentSvc" , true );
    inc -> addListener ( this , IncidentType::BeginEvent ) ;
  }

  // the actual tuning/decoding 
  sc = updateMajor () ;
  if ( sc.isFailure () ) 
  { return Error ( "Error from updateMajor", sc ) ; }  // RETURN

  
  // deal with the histograms:
  sc = updateHistos () ;
  
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the standard execution      of the algorithm 
// ============================================================================
StatusCode CombineParticles::execute    ()  // standard execution 
{
  setFilterPassed ( false ) ;

  if ( m_to_be_updated1 ) 
  {
    StatusCode sc = updateMajor() ;
    if ( sc.isFailure() ) 
    { return Error ( "Error from updateMajor"  , sc ) ; }  // RETURN
  }
  if ( m_to_be_updated2 ) 
  {
    StatusCode sc = updateHistos () ;
    if ( sc.isFailure() ) 
    { return Error ( "Error from updateHistos" , sc ) ; }  // RETURN
  }


  // the local storage of selected particles 
  typedef LoKi::Selected_<LHCb::Particle::ConstVector>  Selected ;
  
  // the looping/combiner engine
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector>  Combiner ;
  
  // get the particles from the Desktop
  const LHCb::Particle::ConstVector& particles 
    = m_useInputParticles ? m_inputParticles : desktop()->particles () ;
  
  if ( m_useInputParticles && msgLevel ( MSG::INFO ) )
  {
    info () 
      << " The external set of " << particles.size() 
      << " particles is used instead of PhysDesktop " 
      << endreq ; 
  }    
  if ( msgLevel ( MSG::VERBOSE ) ) 
  {
    for ( LHCb::Particle::ConstVector::const_iterator i = particles.begin(); i!=particles.end();++i)
      verbose() << "Input :  " << (*i)->key() << " " << (*i)->particleID().pid() << " " 
                << (*i)->momentum() << endmsg ;
  }
  
  // (pre)select all daughters:
  Selected daughters ;
  for ( MyCuts::const_iterator idau = m_cuts.begin() ; m_cuts.end() != idau ; ++idau )
  { 
    Selected::Range r = daughters.add 
      ( idau->m_name       , 
        particles.begin () , 
        particles.end   () , 
        LoKi::Particles::Identifier() == idau->m_pid && idau->m_cut  ) ;
    // some statistics:
    counter( "# " + r.name () ) += r.size() ;
    if (msgLevel(MSG::VERBOSE)){
      for ( Selected::Range::const_iterator i = r.begin(); i!=r.end();++i)
        verbose() << "Select: " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                  << (*i)->momentum() << endmsg ;
    }
    // make plots ? 
    if ( 0 != m_daughtersPlots ) 
    {
      StatusCode sc = m_daughtersPlots -> fillPlots ( r.begin() , r.end() , r.name() ) ;
      if ( sc.isFailure() ) { Warning ("The error from DaughterPlots" , sc ) ; } 
    }
  }
  
  // get the particle combiner/creator 
  //  IParticleCombiner* combiner = particleCombiner() ;
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // loop over all decays 
  for ( LHCb::Decays::const_iterator idecay = m_decays.begin() ; 
        m_decays.end() != idecay ; ++idecay ) 
  {
    // the counter of "good" selected decays 
    size_t nGood = 0 ;                        // the counter of "good" selected decays 
    
    // comparison criteria to remove of double counts for the same pid 
    const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;
    
    // create the actual object for looping:
    
    Combiner loop ;
    
    // fill it with the input data:
    const LHCb::Decay::Items& items = idecay->children() ;
    for ( LHCb::Decay::Items::const_iterator child = items.begin() ;
          items.end() != child ; ++child ) { loop.add ( daughters ( child->name() ) ) ; } 
    
    // here we can start the actual looping
    for ( ; loop.valid() ; loop.next() )  // we are in the loop!
    {
      if ( !loop.unique ( compare ) ) { continue ; }                // CONTINUE 
      
      // get the actual('current') combination:
      LHCb::Particle::ConstVector combination ( loop.dim() ) ;

      loop.current ( combination.begin() ) ;

      if (msgLevel(MSG::VERBOSE)){
        for ( LHCb::Particle::ConstVector::const_iterator i = combination.begin(); i!=combination.end();++i)
          verbose() << "New Com: " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                    << (*i)->momentum() << endmsg ; 
      }
      if (checkOverlap()->foundOverlap(combination)) {
        if (msgLevel(MSG::VERBOSE)) verbose() << "    Overlap!" << endmsg ;
        continue ;
      } 
      if (msgLevel(MSG::VERBOSE)) verbose() << "    No Overlap" << endmsg ;

      // here we have the combination and can apply the cut:
      
      if ( !m_acut ( combination ) )  { 
        if (msgLevel(MSG::VERBOSE)) verbose() << "    Failed Cut!" << endmsg ;
        continue ; }               // CONTINUE      
      if (msgLevel(MSG::VERBOSE)) verbose() << "    Good combination" << endmsg ;
      
      // here we can create the particle (and the vertex)
      
      LHCb::Vertex   vertex   ;
      LHCb::Particle mother ( idecay->mother().pid() ) ;
      
      //      StatusCode sc = combiner -> combine ( combination , mother , vertex ) ;
      StatusCode sc = vertexFitter()->fit( combination , mother , vertex ) ;
      if ( sc.isFailure() ) 
      { 
        Warning ( "The error from IParticleCombiner, skip the combination" , sc ) ; 
        continue ;                                                 // CONTINUE 
      }
      
      if (msgLevel(MSG::VERBOSE)) verbose() << "    Passed fit" << endmsg ;
      if ( 0 != m_combinationPlots ) 
      {
        StatusCode sc = m_combinationPlots->fillPlots ( &mother ) ;
        if ( sc.isFailure() ) { Warning ( "Error from CombinationPlots" , sc ) ; } 
      }
      
      // apply the cut on "mother" particle
      
      if ( !m_cut  ( &mother ) )  { continue ; }                    // CONTINUE
      
      // keep the good candidate:
      
      const LHCb::Particle* particle = desktop()->keep ( &mother ) ;
      
      if ( 0 != m_motherPlots ) 
      {
        StatusCode sc = m_motherPlots->fillPlots ( particle ) ;
        if ( sc.isFailure() ) { Warning ( "Error from MotherPlots" , sc ) ; } 
      }

      // increment number of good decays 
      ++nGood       ;
    } // the loop over combinations
    
    // some statistics
    counter ( "#" + idecay->toString() ) += nGood ;
    nTotal += nGood ;
    
  } // the loop over the decays
  
  // the final statistics 
  counter ( "# selected") += nTotal ;
  
  // reset the "use external input" flag
  m_useInputParticles = false ;
  
  StatusCode sc =  desktop()->saveDesktop() ;
  if (!sc) { return sc ; }

  // the final decision 
  setFilterPassed ( 0 < nTotal ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  Query interfaces of Interface
 *  @param iid ID of Interface to be retrieved
 *  @param iif Pointer to Location for interface pointer
 */
// ============================================================================
StatusCode CombineParticles::queryInterface
(const InterfaceID& iid ,
 void**             iif ) 
{
  if ( 0 == iif ) { return StatusCode::FAILURE ; }                   // RETURN
  //
  if      ( ISetInputParticles::interfaceID () . versionMatch ( iid ) )
  { *iif = static_cast<ISetInputParticles*>      ( this ) ; }
  else if ( IIncidentListener::interfaceID  () . versionMatch ( iid ) )
  { *iif = static_cast<IIncidentListener*>       ( this ) ; }
  else { return Algorithm::queryInterface ( iid , iif ) ; }         // RETURN 
  //
  addRef() ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  Inform that a new incident has occured
 *  @see IIncidentListener 
 */
// ============================================================================
void CombineParticles::handle ( const Incident& )
{
  m_inputParticles.clear()   ;          // clear the input container 
  m_useInputParticles= false ;          // reset the flag
}
// ============================================================================
/** set the input particles
 *  @see ISetInputParticles 
 *  @param input the vector of input particles 
 *  @return status code 
 */
// ============================================================================
StatusCode CombineParticles::setInput 
( const LHCb::Particle::ConstVector& input )
{
  m_inputParticles    = input ;
  m_useInputParticles = true  ;
  info () 
    << "The external set of " << input.size() 
    << " particles will be used as input instead of PhysDesktop " 
    << endreq ;
  return StatusCode ( StatusCode::SUCCESS , true ) ;
}
// =============================================================================
// the handle for the property update
// =============================================================================
void CombineParticles::propertyHandler1 ( Property& p ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_to_be_updated1 = true ;
  Warning ( "The structural property '" + p.name() + 
            "' is updated. It will take affect at the next 'execute'" , 
            StatusCode( StatusCode::SUCCESS, true ) ) ;
  info () << "The updated property is: " << p << endreq ;
}
// =============================================================================
// the handle for the property update
// =============================================================================
void CombineParticles::propertyHandler2 ( Property& p ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_to_be_updated2 = true ;
  debug () << "The histogramming property is updated: " << p << endreq ;
}
// ============================================================================
// perform the update of the major properties 
// ============================================================================
StatusCode CombineParticles::updateMajor  () 
{
  // 1) decode all decay descriptors:
  {
    if ( m_decayDescriptors.empty() ) 
    { m_decayDescriptors.push_back ( getDecayDescriptor() ) ; }
    // locate the decoder tool:
    IDecodeSimpleDecayString* decoder = 
      tool <IDecodeSimpleDecayString> ( "DecodeSimpleDecayString:PUBLIC" , this ) ;
    //
    m_decays = DaVinci::decays ( m_decayDescriptors , decoder ) ;
    if ( m_decays.empty() )
    { return Error ( "Unable to decode DecayDescriptor(s) " ) ; }    // RETURN 
    // release the tool (not needed anymore) 
    release ( decoder );
  }
  // 2) check the list of daughters, involved into all decays:
  {
    typedef std::set<std::string> PIDs ;
    PIDs pids ;
    for ( LHCb::Decays::const_iterator idecay = m_decays.begin() ; 
          m_decays.end()  != idecay ; ++idecay ) 
    {
      const LHCb::Decay::Items& ds= idecay->daughters () ;
      for ( LHCb::Decay::Items::const_iterator ic = ds.begin() ; 
            ds.end ()  != ic ; ++ic ) { pids.insert ( ic->name()  ) ; }  
    }
    // default cut: accept all
    if  ( m_daughterCuts.end() == m_daughterCuts.find ("") ) { m_daughterCuts[""] = "ALL" ; }
    // check that all daughters has the cuts associated:
    for ( PIDs::const_iterator ipid = pids.begin() ; pids.end () != ipid ; ++ipid ) 
    {
      // get the antiparticle:
      const std::string anti = LoKi::Particles::antiParticle ( *ipid ) ;
      // find the cuts for particle 
      Map::const_iterator i1 = m_daughterCuts.find ( *ipid ) ;
      // find the cuts for antiparticle 
      Map::const_iterator i2 = m_daughterCuts.find (  anti ) ;
      //
      // both cuts are already specified explicitly?
      if      ( m_daughterCuts.end() != i1 &&  m_daughterCuts.end() != i2 ) { } // nothing to do
      else if ( m_daughterCuts.end() != i1 ) // the cut for particle is already specified
      { m_daughterCuts[ anti ] = m_daughterCuts[ *ipid ] ; } // use the same cust for antiparticle
      else if ( m_daughterCuts.end() != i2 ) // the cut for anti-particle is already specified 
      { m_daughterCuts[ *ipid ] = m_daughterCuts[ anti ] ; } // use the same cuts as particle
      else // none cuts for particle&antiparticle are specified: use the default cuts  
      {
        m_daughterCuts [ *ipid ] = m_daughterCuts[""] ;  // the default cuts for particle 
        m_daughterCuts [ anti  ] = m_daughterCuts[""] ;  // the default cuts for antiparticle 
      }
    }
    debug () << "The updated map of daughetr cuts is : "
             << Gaudi::Utils::toString ( m_daughterCuts ) << endreq ;
  }
  // 3) decode the cuts for the daughters: 
  { 
    StatusCode sc = decodeAllCuts () ;
    if ( sc.isFailure() ) 
    { return Error ( "The error from decodeAlLCuts", sc ) ; }       // RETURN 
  }
  // 
  m_to_be_updated1 = false ; 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the update of the histogram properties 
// ============================================================================
StatusCode CombineParticles::getHistoTool( IPlotTool*& histoTool, std::string name, std::string path)
{
  if ( validPlots( name )){
    histoTool = tool<IPlotTool>( name, this ) ;
    if ("" != path ){
      StatusCode sc = m_daughtersPlots -> setPath ( path ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to set Plots Path "+path+" for tool "+name , sc ) ; }
    }
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the update of the histogram properties 
// ============================================================================
StatusCode CombineParticles::updateHistos () 
{
  // ==========================================================================
  if ( 0 != m_daughtersPlots   ) 
  { releaseTool ( m_daughtersPlots   ) ; m_daughtersPlots   = 0 ; }
  if ( 0 != m_combinationPlots ) 
  { releaseTool ( m_combinationPlots ) ; m_combinationPlots = 0 ; }
  if ( 0 != m_motherPlots      ) 
  { releaseTool ( m_motherPlots      ) ; m_motherPlots      = 0 ; }
  // ==========================================================================  
  if ( produceHistos () ) 
  {
    StatusCode sc = getHistoTool( m_daughtersPlots, m_daughtersPlotsName, m_daughtersPlotsPath );
    if (sc) sc = getHistoTool( m_combinationPlots, m_combinationPlotsName, m_combinationPlotsPath );
    if (sc) sc = getHistoTool( m_motherPlots, m_motherPlotsName, m_motherPlotsPath );
    if (!sc) return sc ;
  }
  // ========================================================================
  // 
  m_to_be_updated2 = false ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// The factory (needed for the proper instantiation)
DECLARE_ALGORITHM_FACTORY(CombineParticles);
// ============================================================================
// The END 
// ============================================================================
