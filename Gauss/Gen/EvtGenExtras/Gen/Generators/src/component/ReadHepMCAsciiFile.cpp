// $Id: ReadHepMCAsciiFile.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2008/07/09 14:37:50  robbep
// Gaudi Units
//
// Revision 1.3  2007/03/08 13:41:06  robbep
// Adapt to new production tool interface
//
// Revision 1.2  2007/01/12 15:17:39  ranjard
// v7r0 - use GAUDI v19r0
//
// Revision 1.1  2006/10/06 14:11:17  ibelyaev
//  add (Read,Write)HepMCAsciiFile components
//
// ===========================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
// ===========================================================================
// GaudiAlg
// ===========================================================================
#include "GaudiAlg/GaudiTool.h"
// ===========================================================================
// Generators 
// ===========================================================================
#include "Generators/IProductionTool.h"
// ===========================================================================
// HepMC 
// ===========================================================================
#include "HepMC/IO_Ascii.h"
// ===========================================================================
// Local 
// ===========================================================================
#include "Generators/Scale.h"
// ===========================================================================
/** @class ReadHepMCAsciiFile ReadHepMCAsciiFile.cpp
 *
 *  Simple "production tool", which actually reads HepMC events 
 *  from  ascii input file. The tool could be useful to read 
 *  information from external generators, if their integration 
 *  into Gauss is impossible, difficult or not needed. 
 *  
 *  The tool has 2 properties:
 *
 *    - <c>Input</c>  : The name of input fiel in HepMC Ascii format 
 *                    ( the default value is <c>""</c> (empty string))
 *    - <c>Rescale<c> : Boolean flag to rescal the input event from 
 *                     Pythia units to LHCb units.
 *                     The default value is <c>true</c>.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2005-10-05
 */
class ReadHepMCAsciiFile 
  :         public       GaudiTool
  , virtual public IProductionTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<ReadHepMCAsciiFile> ;
public:
  /// initialization of the tool 
  virtual StatusCode initialize () 
  {
    StatusCode sc = GaudiTool::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    /// check the output file 
    if ( m_input.empty() ) 
    { return Error ( "Input file name is not specified!" ) ; }
    // open the file 
    m_file = new HepMC::IO_Ascii ( m_input.c_str() , std::ios::in ) ;
    //  
    if ( 0 == m_file || m_file->rdstate() == std::ios::failbit ) 
    { return Error ( "Failure to input the file '"+m_input+"'" ) ; }
    //
    return StatusCode::SUCCESS ;
  }
  /// finalization of the tool 
  virtual StatusCode finalize   () 
  {
    // delete the stream  (close the file!)
    if ( 0 != m_file ) { delete m_file ; m_file = 0 ; }
    // finalize the base class ;
    return GaudiTool::finalize() ;
  }
public:
  virtual StatusCode initializeGenerator() { return StatusCode::SUCCESS ; } 
  
  // ===================================================================
  /** Generate a primary interaction. 
   *  The generated event contains all what happens up to the decay of
   *  hadrons. Decay of hadrons will be performed in general by the decay
   *  tool. Then all hadrons must be declared stable in the production
   *  generator. This is done thanks to IProductionTool::setStable.
   *  @param[out] theEvent  Event generated by the production generator.
   *  @param[out] theInfo   Informations about the hard process of the
   *                        generated interaction.
   */
  virtual StatusCode generateEvent 
  ( HepMC::GenEvent    * theEvent , 
    LHCb::GenCollision * theInfo  ) ;
  // ===================================================================
  /// Declare a particle stable to the production generator.
  virtual void setStable
  ( const ParticleProperty * /* thePP */ ) { }
  // ===================================================================
  /** Update the properties of the particle in the production generator
   *  with values from the particle property service.
   */
  // ===================================================================
  virtual void updateParticleProperties
  ( const ParticleProperty * /* thePP */ ) {};
  // ===================================================================
  /// Turn off the fragmentation step in the generation.
  virtual void turnOffFragmentation  ( ) {} ;
  // ===================================================================
  /// Turn on the fragmentation step in the generation.
  virtual void turnOnFragmentation   ( )  {}  ;
  // ===================================================================
  /** Hadronize the event.
   *  @param[in,out] theEvent  Unfragmented event to hadronize.
   *  @param[out]    theInfo   Informations about the hard process of the 
   *                           generated interaction.
   */
  virtual StatusCode hadronize
  ( HepMC::GenEvent*     /* theEvent */ , 
    LHCb::GenCollision * /* theInfo  */ ) { return StatusCode::SUCCESS ; }
  // ===================================================================
  /// Save the parton level event (when the fragmentation is turned off)
  virtual void savePartonEvent
  ( HepMC::GenEvent * /* theEvent */ ) {} ;
  // ===================================================================
  /// Retrieve the previously saved parton event to re-hadronize it.
  virtual void retrievePartonEvent
  ( HepMC::GenEvent* /* theEvent */ ) {} ;
  // ===================================================================
  /// Print configuration of production generator 
  virtual void printRunningConditions( ) {} ;
  // ===================================================================
  /** Define special particles whose properties must not be updated from 
   *  the particle property service (like mass of top quark, ...)
   */
  virtual bool isSpecialParticle
  ( const ParticleProperty* /* thePP */ ) const { return true ; }
  // ===================================================================
  /// Prepare the generator to force fragmentationto in the thePdgId flavour.
  virtual StatusCode setupForcedFragmentation
  ( const int /* thePdgId */ ) { return StatusCode::SUCCESS ; };
  // ===================================================================
protected:
  /** standard constructor
   *  @param type tool type(?)
   *  @param name tool name
   *  @param parent parent  component 
   */
  ReadHepMCAsciiFile
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent )
    : GaudiTool ( type , name , parent ) 
    // no default 
    , m_input () ///< no default input file name 
    // rescale from Pythia units to LHCb units 
    , m_rescale ( true ) ///< rescale from Pythia units to LHCb units 
    // the fiel itself 
    , m_file    ( 0    ) ///< the fiel itself 
  {
    declareInterface<IProductionTool> ( this ) ;
    
    declareProperty  ( "Input"   , m_input   ) ;
    declareProperty  ( "Rescale" , m_rescale ) ;
  } 
  /// virtual and protected destrcutor
  virtual ~ReadHepMCAsciiFile() 
  {
    // delete the stream  (close the file!)
    if ( 0 != m_file ) { delete m_file ; m_file = 0 ; }
  };
private: 
  // defaut constructor is disabled 
  ReadHepMCAsciiFile() ;
  // defaut constructor is disabled 
  ReadHepMCAsciiFile( const ReadHepMCAsciiFile& ) ;
  // assignement operator is disabled 
  ReadHepMCAsciiFile& operator=( const ReadHepMCAsciiFile& ) ;
private:
  // the name of the input file 
  std::string      m_input ; ///< the name of the input file 
  // rescale event from Pythia to LHCb units ?
  bool             m_rescale ; ///< rescale event to LHCb units ?  
  // the output file ;
  HepMC::IO_Ascii* m_file   ; ///< the input file ;
} ;
// =====================================================================
/// Declaration of the Tool Factory
// =====================================================================
DECLARE_TOOL_FACTORY( ReadHepMCAsciiFile );
// =====================================================================
/** Generate a primary interaction. 
 *  The generated event contains all what happens up to the decay of
 *  hadrons. Decay of hadrons will be performed in general by the decay
 *  tool. Then all hadrons must be declared stable in the production
 *  generator. This is done thanks to IProductionTool::setStable.
 *  @param[out] theEvent  Event generated by the production generator.
 *  @param[out] theInfo   Informations about the hard process of the
 *                        generated interaction.
 */

// ===================================================================
StatusCode ReadHepMCAsciiFile::generateEvent 
( HepMC::GenEvent    *    theEvent , 
  LHCb::GenCollision * /* theInfo */ ) 
{
  Assert ( 0 != m_file , "Invalid input file!" ) ;
  //
  if ( !m_file->fill_next_event( theEvent ) ) 
  { return Error ( "Error in event reading!" ) ; }
  // rescale if needed (convert to LHCb units) 
  if ( m_rescale ) 
    { GeneratorUtils::scale ( theEvent , Gaudi::Units::GeV ,
                              Gaudi::Units::mm / Gaudi::Units::c_light ) ; }
  //
  return StatusCode::SUCCESS ;
} ;
// ===================================================================


// =====================================================================
// The END 
// =====================================================================
