// $Id: CaloHypo2TrackAlg.cpp,v 1.7 2006-06-14 16:49:22 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/05/30 09:42:02  odescham
// first release of the CaloReco migration
//
// Revision 1.5  2005/11/07 12:12:42  odescham
// v3r0 : adapt to the new Track Event Model
//
//
// Revision 1.5  2005/17/10 09:45:56  odescham
// adapt to new track model
//
// Revision 1.4  2004/10/26 20:35:58  ibelyaev
//  improve properties of all Track-related algorithms
//:
// ============================================================================
// Include files
// ============================================================================
// Relations 
#include "Relations/RelationWeighted2D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event 
#include "Event/CaloHypo.h"
#include "Event/Track.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackMatch.h"
// CaloUtils
#include "CaloUtils/Calo2Track.h"
// local
#include "CaloHypo2TrackAlg.h"

// ============================================================================
/** @file
 * 
 *  implementation file for class CaloHypo2TrackAlg
 *  @see CaloHypo2TrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloHypo2TrackAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypo2TrackAlg>         s_Factory ;
const        IAlgFactory&CaloHypo2TrackAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloHypo2TrackAlg::CaloHypo2TrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloTrackAlg  ( name , svcloc ) 
  , m_tracks      ( LHCb::TrackLocation::Default )
  , m_cut         ( 1.e+30     )
  //
  , m_matchType   ( "SomeType" ) 
  , m_matchName   ( ""         ) 
  , m_match       ( 0      )
  , m_inputData ()
  , m_outputData()  
{
  declareProperty( "Tracks"     , m_tracks     ) ;
  declareProperty( "MatchType"  , m_matchType  ) ;
  declareProperty( "MatchName"  , m_matchName  ) ;
  declareProperty( "Cut"        , m_cut        ) ;
  declareProperty ( "InputData"      , m_inputData ) ;
  declareProperty ( "OutputData"     , m_outputData) ;  
  // 
  // 'electron'-match configuration 
  setProperty     ( "SkipClones"     , "true"      ) ;
  setProperty     ( "SkipInvalid"    , "true"      ) ;
  setProperty     ( "SkipBackward"   , "true"      ) ;
  setProperty     ( "CheckTracks"    , "true"      ) ;
  //
  std::vector<int> types;
  types.push_back(LHCb::Track::Long);
  types.push_back(LHCb::Track::Downstream);
  types.push_back(LHCb::Track::Ttrack);
  IntegerArrayProperty ptyp("AcceptedType", types);
  setProperty     (  ptyp ) ;

};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloHypo2TrackAlg::~CaloHypo2TrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypo2TrackAlg::initialize() 
{
  // initialize the base class 
  StatusCode sc = CaloTrackAlg::initialize();
  if ( sc.isFailure() ) 
  { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  // retrieve the tool from Tool service 
  m_match = tool<ICaloTrackMatch>( m_matchType , m_matchName );
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypo2TrackAlg::execute() 
{
  // avoid long names 
  typedef const LHCb::CaloHypos                                         Hypos    ;
  typedef const LHCb::CaloHypo                                          Hypo     ;
  typedef  LHCb::RelationWeighted2D< LHCb::CaloHypo , LHCb::Track , float >   HypoTrTable;
  
  // get Hypos from Transient Store  
  Hypos*    hypos    = get<Hypos>    ( m_inputData ) ;
  
  // get tracks   from Transient Store  
  LHCb::Tracks*   tracks   = get<LHCb::Tracks>   ( m_tracks    );
  

  // create relation table and register it in the store
  HypoTrTable*    table = new HypoTrTable();
  put( table , m_outputData );
  
  if ( 0 == tracks   -> size () )
  { Warning ( "Empty container of Tracks " , StatusCode::SUCCESS ) ; }
  if ( 0 == hypos    -> size () )
  { Warning ( "Empty container of Hypos  " , StatusCode::SUCCESS ) ; }


  
  if( 0 == tracks   -> size () || 
      0 == hypos    -> size () )     { return StatusCode::SUCCESS ; }
  
  // loop over tracks  
  for ( LHCb::Tracks::const_iterator track = tracks->begin() ; 
        tracks->end() != track ; ++track )
  {  
    // skip NULLS 
    if ( 0 == *track            ) { continue ; }             // CONTINUE
    
    // use track ?
    if ( ! use( *track )       ) { continue ; }             // CONTINUE 
    
    // loop over hypos 
    for ( Hypos::const_iterator hypo = hypos->begin() ; 
          hypos->end() != hypo ; ++hypo )
    {
      // skip NUULs 
      if ( 0 == *hypo          ) { continue ; }             /// CONTINUE 
      
      // valid hypo ? 
      if ( 0 == (*hypo)->position() )
      {
        Warning("CaloPosition* points to NULL!");
        continue ;                                      /// CONTINUE 
      }
      
      // perform the matching
      double chi2 = 0 ;
      StatusCode sc = m_match -> match( (*hypo)->position() , *track , chi2 );
      
      if ( sc.isFailure() )
      {
        Warning("Hypo/Track: matching failure, skip pair" );
        continue ;
      }
      else if ( 0 <= chi2 && chi2 <=  m_cut ) 
      { table -> relate( *hypo , *track , chi2 ); }
      
    }; // loop over all hypos 
    
  }; // loop over all tracks 
  
  if ( msgLevel( MSG::DEBUG ) ) 
  { debug() << "Entries in the table " << table->relations().size() << endreq ; }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

