// $Id: CaloClustersMCTruthAlg.cpp,v 1.13 2006-05-17 16:11:59 cattanem Exp $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
//  LHCbKernel 
// ============================================================================
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationWeighted1D.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// CaloEvent/Event
// ============================================================================
#include "Event/CaloCluster.h"
//CaloUtils
#include "CaloUtils/CaloMCTools.h"
#include "CaloUtils/Calo2MC.h"
// ============================================================================

/** @class CaloClustersMCTruthAlg CaloClustersMCTruthAlg.h
 *  
 *  The algorithm for building the relations
 *  between CaloClusters and MCParticles
 *
 *  This implementation is "primitive" - it relate
 *  CaloCluster object with MCParticle, which HAS a deposition 
 *  of active energy with exceed of some threshold.
 *  the deposited energy is the "weight" of the relation
 * 
 *  @see CaloCluster
 *  @see MCParticle
 *  @see Relation2D 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */
class CaloClustersMCTruthAlg : public GaudiAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<CaloClustersMCTruthAlg>; 
public:

  /** standard intialization of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode initialize () 
  {
    StatusCode sc = GaudiAlgorithm::initialize() ;
    if ( sc.isFailure  () )  { return sc ; }
    if ( m_inputs.empty() ) 
    { Warning ( "Empty input list of CalolLuster conainers" )  ; }  
    return StatusCode::SUCCESS ;
  } ;
  
  /** standard execution of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode execute    () ;

protected:
  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  CaloClustersMCTruthAlg
  ( const std::string& name , 
    ISvcLocator*       svc  )
    : GaudiAlgorithm    ( name , svc                  ) 
    , m_threshold       ( 2 * Gaudi::Units::perCent   ) 
    // location(s) of clusters 
    , m_inputs          ()
    , m_output          ( "Relations/" + LHCb::CaloClusterLocation::Default + "CCs2MCPs")
    , m_detector        ( DeCalorimeterLocation::Ecal              )  
    , m_particles       ( LHCb::MCParticleLocation::Default        ) 
  { 
    declareProperty     ( "Threshold"   , m_threshold  ) ;
    //
    m_inputs.push_back  ( LHCb::CaloClusterLocation::Ecal      ) ;
    m_inputs.push_back  ( LHCb::CaloClusterLocation::EcalSplit ) ;    
    //
    declareProperty     ( "Inputs"      , m_inputs     ) ;
    declareProperty     ( "Output"      , m_output     ) ;
    //
    declareProperty     ( "MCParticles" , m_particles  ) ;
    declareProperty     ( "Detector"    , m_detector   ) ;
  };
  
  /// destructor (virtual and protected)
  virtual ~CaloClustersMCTruthAlg(){};
  
private:
  
  /// default  constructor is private
  CaloClustersMCTruthAlg();
  /// copy     constructor is private
  CaloClustersMCTruthAlg
  ( const CaloClustersMCTruthAlg& );
  /// assignement operator is private
  CaloClustersMCTruthAlg& operator=
  ( const CaloClustersMCTruthAlg& );
  
private:
  
  //  energy fratcion threshold
  double      m_threshold ; ///<  energy fratcion threshold
  //
  typedef std::vector<std::string> Inputs ;
  //  list of input CaloCluster containers 
  Inputs      m_inputs    ; ///< list of input CaloCluster containers 
  // TES address of output relaiton table 
  std::string m_output    ; ///< TES address of output relaiton table 
  // Detector element ot be used 
  std::string m_detector  ; ///< Detector element ot be used 
  // location of MC-particles
  std::string m_particles ; ///< location of MC-particles
  //
};

// ============================================================================
/** @file CaloClustersMCTruthAlg.cpp
 * 
 *  Implementation file for class : CaloClustersMCTruthAlg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY(CaloClustersMCTruthAlg);
// ============================================================================

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruthAlg::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  using namespace CaloMCTools;
  typedef const LHCb::CaloClusters                 Clusters  ;
  typedef const DeCalorimeter                      Detector  ;
  typedef const LHCb::MCParticles                  Particles ;
  typedef LHCb::Calo2MC::ClusterTable              Table     ;
  
  // get mc particles 
  Particles*  particles = get<Particles>        ( m_particles ) ;
  
  // get the detector 
  Detector*   detector  = getDet<DeCalorimeter> ( m_detector );
  
  // scale factor for recalculation of Eactive into Etotal 
  const double activeToTotal = detector->activeToTotal() ;
  
  // create relation table and register it in the event transient store 
  Table* table = new Table( 500 );
  put( table , m_output ) ;

  if ( m_inputs.empty() ) 
  { return Error ("No input data are specified! "); } // RETURN 
  
  // create the MCtruth evaluator 
  EnergyFromMCParticle<LHCb::CaloCluster> evaluator;

  for ( Inputs::const_iterator input = m_inputs.begin() ; 
        m_inputs.end() != input ; ++input )
  {
    // check the cluster
    if ( !exist<Clusters>( *input ) ) 
    {
      Error ( "Missing container: '" + (*input) + "' - skip" ) ;  
      continue ;                                               // CONTINUE      
    }
    
    // get input clusters 
    Clusters*   clusters  = get<Clusters>         ( *input ) ;
    
    // loop over all clusters 
    for ( Clusters::const_iterator cluster = clusters->begin() ;
          clusters->end() != cluster ; ++cluster ) 
    {
      // Skip NULLs
      if ( 0 == *cluster ) { continue ; }                        // CONTINUE 
      // define "current cut" value 
      const double  cut = m_threshold * (*cluster)->e() ;
      // loop over all MC truth particles 
      for ( Particles::const_iterator    particle = particles->begin() ; 
            particles->end() != particle ; ++particle )
      {    
        // Skip NULLS  
        if ( 0 == *particle ) { continue ; }                     // CONTINUE  
        // Skip low momentum particles  
        if ( (*particle)->momentum().e() < 0.90 * cut ) { continue ; }
        // use the evaluator to extract exact MCTruth information 
        const double  energy =  
          evaluator( *cluster , *particle ) * activeToTotal ;
        // skip small energy depositions
        if (  cut > energy  ) { continue ; }                     // CONTINUE
        // put relation to relation table 
        // ATTENTION: i_push is in use 
        table->i_push ( *cluster , *particle , (float) energy ); // PUSH!
        //
      }; // end of loop over MC particles 
      //
    } // end of loop over clusters 
    //
  } // end of loop over cluster containers 
  
  // MANDATORY CALL FOR i_sort() after i_push 
  table->i_sort() ;                                        // NB: i_sort
  
  // count number of links 
  counter ( "#CC2MC links" ) + table->relations().size() ;
  
  if ( table->relations().empty() ) 
  { Warning ( " The relations table '" + m_output + "' is empty!") ; }
  
  if ( msgLevel ( MSG::DEBUG )  ) 
  {
    debug() << " Number of established relations are #"
            << table->relations().size() << endreq ;
  }

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
