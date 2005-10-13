// Include files
// -------------
// LHCbKernel
#include "Relations/RelationWeighted2D.h"

// from LHCbEvent
#include "Event/EventHeader.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/ITMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// Local
#include "IdealTracksCreator.h"

static const AlgFactory<IdealTracksCreator>    s_factory;
const IAlgFactory& IdealTracksCreatorFactory = s_factory;

/** @file IdealTracksCreator.cpp
 *
 *  Implementation of IdealTracksCreator.
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model, 18-04-2005)
 *  @author M. Needham
 *  @author R.M. van der Eijk (28-5-2002) 
 *  @author J. van Tilburg (02-07-2003)
 *  @date   27-7-1999
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IdealTracksCreator::IdealTracksCreator( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : GaudiAlgorithm( name, pSvcLocator )
  , m_otTim2MCHit(0)
  , m_itClus2MCP(0)
  , m_veloClus2MCP(0)
  , m_otTracker(0)
  , m_itTracker(0)
  , m_velo(0)
  , m_trackSelector(0)
  , m_stateCreator(0)
//  , m_tracksFitter(0)
//  , m_veloTracksFitter(0)
//  , m_seedTracksFitter(0)
{
  /// default job Options
  declareProperty( "AddOTTimes",      m_addOTTimes = true );
  declareProperty( "AddITClusters",   m_addITClusters = true );
  declareProperty( "AddVeloClusters", m_addVeloClusters = true );
  declareProperty( "InitState",       m_initState = true );
  declareProperty( "FitTracks",       m_fitTracks = true );
  declareProperty( "FitUpstream",     m_upstream = true );
  declareProperty( "TrueStatesAtMeasZPos", m_trueStatesAtMeas = false );
  declareProperty( "TracksTESPath",
                   m_tracksTESPath = "Rec/Track/Ideal" );
  declareProperty( "RelationTable",
                   m_relationTable = "Rec/Relations/IdealTr2MCP" );
  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default );
  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = DeSTDetectorLocation::Default );
  declareProperty( "VeloGeometryPath",
                   m_veloPath = "/dd/Structure/LHCb/Velo" );
  declareProperty( "MinNHits", m_minNHits = 6 );
  declareProperty( "ErrorX2",  m_errorX2  = 4.0 );
  declareProperty( "ErrorY2",  m_errorY2  = 400.0 );
  declareProperty( "ErrorTx2", m_errorTx2 = 6.e-5 );
  declareProperty( "ErrorTy2", m_errorTy2 = 1.e-4 );
  declareProperty( "ErrorP",   m_errorP   = 0.15 );
}

//=============================================================================
// Destructor
//=============================================================================
IdealTracksCreator::~IdealTracksCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode IdealTracksCreator::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  // Retrieve the MC associators
  // ---------------------------
  sc = toolSvc() -> retrieveTool( "OTTime2MCHitAsct", m_otTim2MCHit );
  if ( !sc ) { return sc; }
  sc = toolSvc() -> retrieveTool( "ITCluster2MCParticleAsct", m_itClus2MCP );
  if ( !sc ) { return sc; }
  sc = toolSvc() -> retrieveTool( "VeloCluster2MCParticleAsct", m_veloClus2MCP );
  if ( !sc ) { return sc; }
  debug() << "Associators retrieved." << endreq;

  // Load Geometry from XmlDDDB
  // --------------------------
   m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );

   m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );

   m_velo      = getDet<DeVelo>( m_veloPath );
   debug() << "Geometry read in." << endreq;

  // Retrieve the TrackCriteriaSelector tool
   m_trackSelector = tool<ITrackCriteriaSelector>( "TrackCriteriaSelector",
                                                   "select", this );

  // Retrieve the IdealStateCreator tool
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator" );

  if ( m_fitTracks ) {
    always() << "Fitting part of algorithm not yet done!" << endreq;
//    sc = toolSvc()->retrieveTool( "TrFitAtAllPoints", "Fitter", 
//                                  m_tracksFitter, this );
//    if ( !sc ) { return sc; }
//    sc = toolSvc()->retrieveTool( "TrFitAtAllPoints", "SeedFitter", 
//                                  m_seedTracksFitter, this );
//    if ( !sc ) { return sc; }
//    sc = toolSvc()->retrieveTool( "TrFitAtAllPoints", "VeloFitter", 
//                                  m_veloTracksFitter, this );
//    if ( !sc ) { return sc; }
//    sc = toolSvc()->retrieveTool( "TrFitAtAllPoints", "VeloTTFitter", 
//                                  m_veloTTTracksFitter, this );
//    if ( !sc ) { return sc; }
  }

  info() << "initialized succesfully" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode IdealTracksCreator::execute()
{
  debug() << "==> Execute" << endreq;

  // Event header info
  EventHeader* evtHdr = get<EventHeader>( EventHeaderLocation::Default );
  debug() << "Run " << evtHdr -> runNum()
          << "\tEvent " << evtHdr -> evtNum() << endreq;

  // Retrieve the MCParticle container
  MCParticles* particles = get<MCParticles>( MCParticleLocation::Default );

  /// Retrieve the Containers for Clusters and Times
  OTTimes* otTimes           = get<OTTimes>( OTTimeLocation::Default );
  debug() << "- retrieved " <<  otTimes -> size() << " OTTimes." << endreq;

  // Make container for tracks
  Tracks* tracksCont = new Tracks();

  // create relation table and register it in the event transient store
  typedef RelationWeighted2D<Track,MCParticle,double>  Table;
  Table* table = new Table();
  StatusCode sc = eventSvc() -> registerObject( m_relationTable, table );
  if( sc.isFailure() ) {
    error() << "Unable to register the relation container = "
            << m_relationTable << " status = " << sc << endreq;
    return sc ;
  }
  else {
    verbose() << "Relations container " << m_relationTable
              << " registered" << endreq;
  }

  debug() << "Starting loop over the "
          << particles -> size() << " MCParticles ... " << endreq;

  // loop over MCParticles
  // =====================
  MCParticles::const_iterator iPart;
  for ( iPart = particles->begin(); particles->end() != iPart; ++iPart ) {
    MCParticle* mcParticle = *iPart;
    verbose() << endreq
            << "- MCParticle of type "
            << m_trackSelector -> trackType( mcParticle )
            << " , (key # " << mcParticle -> key() << ")" << endreq
            << "    - momentum = " << mcParticle -> momentum() << " MeV" << endreq
            << "    - P        = " << mcParticle -> momentum().vect().mag()
            << " MeV" <<endreq
            << "    - charge   = "
            << ( mcParticle -> particleID().threeCharge() / 3 ) << endreq;
    if ( m_trackSelector -> select( mcParticle ) ) {
      debug() << endreq
              << "Selected MCParticle of type "
              << m_trackSelector -> trackType( mcParticle )
              << " , (key # " << mcParticle -> key() << ")" << endreq
              << "    - momentum = " << mcParticle -> momentum() << " MeV" <<endreq
              << "    - P        = " << mcParticle -> momentum().vect().mag()
              << " MeV" <<endreq
              << "    - charge   = "
              << ( mcParticle -> particleID().threeCharge() / 3 ) << endreq;

      // Make a new track
      // ----------------
      Track* track = new Track();

      m_trackSelector -> setTrackType( mcParticle, track );

      // Check whether a Velo track is backward
      if ( Track::Velo == track -> type() ) {
        const double pz = mcParticle -> momentum().pz();
        if ( pz < 0.0 ) track -> setFlag( Track::Backward, true );
      }

      // Add Velo clusters
      // -----------------
      if ( m_addVeloClusters == true ) {
        debug() << "... adding VeloXxxClusters" << endreq;
        sc = addVeloClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add velo R clusters" << endreq;
          return StatusCode::FAILURE;
        }
      }

      // Add IT clusters
      // ---------------
      if ( m_addITClusters == true ) {
        debug() << "... adding ITClusters" << endreq;
        sc = addITClusters( mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add inner tracker clusters" << endreq;
          return StatusCode::FAILURE;
        }        
      }
      
      // Add OTTimes
      // -----------
      if ( m_addOTTimes == true && otTimes != 0 ) {
        debug() << "... adding OTTimes" << endreq;
        sc = addOTTimes( otTimes, mcParticle, track );
        if ( sc.isFailure() ) {
          error() << "Unable to add outer tracker OTTimes" << endreq;
          return StatusCode::FAILURE;
        }
      }

      // Check if the track contains enough hits
      // ---------------------------------------
      if ( (int) track -> nMeasurements() < m_minNHits) {
        debug() << " -> track deleted. Had only " << track -> nMeasurements()
                << " hits" << endreq;
        delete track;
        continue; // go to next track
      }

      // Sort the measurements in z
      // --------------------------
      //sortMeasurements( track );  // no more needed!

      // Initialize a seed state
      // -----------------------
      if ( m_initState ) {
        if ( m_upstream ) {
          std::vector<Measurement*>::const_reverse_iterator rbeginM =
            track -> measurements().rbegin();
          sc = this -> initializeState( (*rbeginM)->z(),
                                        track, mcParticle );
        }
        else {
          std::vector<Measurement*>::const_iterator beginM =
            track -> measurements().begin();
          sc = this -> initializeState( (*beginM)->z(),
                                        track, mcParticle );
        }
        if ( !sc.isSuccess() ) {
          delete track;
          continue; // go to next track
        }
      }

      // Set some of the track properties
      // --------------------------------
      track -> setStatus( Track::PatRecMeas );
      track -> setFlag( Track::Unique, true );
      track -> setHistory( Track::TrackIdealPR );
      // Fit the track
      //if ( m_initState && m_fitTracks ) {
        // select appropriate track fitter
        //ITrackFitter* fitter = m_tracksFitter;
        //if( track -> type() == Track::Velo
            //|| track -> checkFlag( Track::Backward ) )
          //fitter = m_veloTracksFitter;
        //if ( track->velo() || track->veloBack() ) fitter = m_veloTracksFitter;
        //if ( track -> type() == Track::Upstream ) fitter = m_veloTTTracksFitter;
        //if ( track->veloTT() ) fitter = m_veloTTTracksFitter;
        //if ( track -> type() == Track::Ttrack ) fitter = m_seedTracksFitter;
        //if ( track->seed() ) fitter = m_seedTracksFitter;
        // Fit the track 
        //if ( m_upstream ) { // do upstream fit
//          sc = fitter->fitUpstream(track, track->rbeginM(), track->rendM() );
        //} else { // do downstream fit
//          sc = fitter->fitDownstream(track, track->beginM(), track->endM() );
        //}
        // Set error flag if fit failed
        //if ( sc.isFailure() ) track -> setStatus( Track::Failed );
        //if ( sc.isFailure() ) track -> setErrorFlag(1);
      //}

      // Add true states at each measurement 
      // ===================================
      if ( m_trueStatesAtMeas ) {
        deleteStates( track );
        sc = StatusCode::SUCCESS;
        std::vector<Measurement*>::const_iterator iMeas =
          track -> measurements().begin();
        std::vector<Measurement*>::const_iterator endM =
          track -> measurements().end();
        //Track::const_measure_iterator iMeas = track->beginM();
        while ( sc.isSuccess() && iMeas != endM ) {
          State* tempState;
          sc = m_stateCreator -> createState( mcParticle,
                                              (*iMeas)->z(),
                                              tempState );
          if ( sc.isSuccess() ) track -> addToStates( *tempState );
          else delete tempState;
          ++iMeas;
        }
        if ( sc.isFailure() ) {
          delete track;
          continue;
        }        
      }

      debug() << "At the end: states at z-positions: ";
      const std::vector<State*>& allstates = track->states();
      for ( unsigned int it = 0; it < allstates.size(); it++ ) {
        debug() << allstates[it] -> z() << " ";
      }
      debug() << endreq;
      for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
        debug() << "state vectors:" << endreq
                << allstates[it2] -> stateVector() << endreq;
      }
      debug() << endreq;

      // Add the track to the Tracks container and fill the relations table
      // ------------------------------------------------------------------
      tracksCont -> add( track );
      table -> relate( track, mcParticle, 1.0 );

      // debugging Track ...
      // -------------------
      debug()
        << "-> Track with key # " << track -> key() << endreq
        << "  * charge         = " << track -> charge() << endreq
        << "  * is Invalid     = " << track -> checkFlag( Track::Invalid ) << endreq
        << "  * is Unique      = " << track -> checkFlag( Track::Unique ) << endreq
        << "  * is of type     = " << track -> type() << endreq
        << "  * is Backward    = " << track -> checkFlag( Track::Backward ) << endreq
        << "  * # measurements = " << track -> nMeasurements() << endreq;
      
      // print the measurements
      const std::vector<Measurement*>& meas = track -> measurements();
      for ( std::vector<Measurement*>::const_iterator itMeas = meas.begin();
            itMeas != meas.end(); itMeas++ ) {
        debug() << "  - measurement of type " << (*itMeas) -> type() << endreq
                << "  - z        = " << (*itMeas) -> z() << " mm" << endreq
                << "  - LHCbID   = " << (*itMeas) -> lhcbID()  << endreq;
        // continue according to type ...  
        if ( (*itMeas) -> lhcbID().isOT() ) {
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().otID() << endreq;
        }
        else if ( (*itMeas) -> lhcbID().isST() ) {
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().stID() << endreq;
        }
        else if ( (*itMeas) -> lhcbID().isVelo() ) {    
          debug() << "  - XxxChannelID = " << (*itMeas) -> lhcbID().veloID() << endreq;
        }
      }

    } // is selected
  } // looping over MCParticles

  // Store the Tracks in the TES
  // ===========================
  sc = registerTracks( tracksCont );
  if( sc.isFailure() ) return sc;

  debug() << "Created " << tracksCont -> size() << " tracks." << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode IdealTracksCreator::finalize()
{
  debug() << "==> Finalize" << endreq;

  // Release all tools and services
  // ------------------------------
  if ( m_veloClus2MCP )     toolSvc()->releaseTool( m_veloClus2MCP );
  if ( m_itClus2MCP )       toolSvc()->releaseTool( m_itClus2MCP );
  if ( m_otTim2MCHit  )     toolSvc()->releaseTool( m_otTim2MCHit );

  //if ( m_tracksFitter )     toolSvc()->releaseTool( m_tracksFitter );
  //if ( m_veloTracksFitter ) toolSvc()->releaseTool( m_veloTracksFitter );
  //if ( m_seedTracksFitter ) toolSvc()->releaseTool( m_seedTracksFitter );

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addOTTimes( OTTimes* times,
                                           MCParticle* mcPart,
                                           Track* track )
{
  unsigned int nOTMeas = 0;

  // Loop over outer tracker clusters
  OTTimes::const_iterator itOTTime = times -> begin();
  while ( itOTTime != times->end() ) {
    OTTime* aTime = *itOTTime;

    // retrieve MCHit associated with this cluster
    OTTime2MCHitAsct::ToRange range = m_otTim2MCHit -> rangeFrom(aTime);
    OTTime2MCHitAsct::ToIterator iTim;
    for ( iTim = range.begin(); iTim != range.end(); ++iTim ) {
      MCHit* aMCHit = iTim -> to();
      MCParticle* aParticle = aMCHit -> mcParticle();
      if ( aParticle == mcPart ) {
        // get the ambiguity from the MCHit
        const OTChannelID channel = aTime -> channel();
        DeOTModule* module = m_otTracker -> module( channel );
        const HepPoint3D entryP = aMCHit -> entry();
        const HepPoint3D exitP = aMCHit -> exit();
        double deltaZ = exitP.z() - entryP.z();
        if (0.0 == deltaZ) continue; // curling track inside layer
        const double tx = (exitP.x() - entryP.x()) / deltaZ;
        const double ty = (exitP.y() - entryP.y()) / deltaZ;
        double mcDist = module -> distanceToWire( channel.straw(),
                                                  entryP, tx, ty );
        int ambiguity = 1;
        if ( mcDist < 0.0 ) ambiguity = -1;
        // Get the tu from the MCHit
        double angle = module -> stereoAngle();
        double tu = tx * cos(angle) + ty * sin(angle);
        // Make the TrMeasurement
        OTMeasurement otTim = OTMeasurement( *aTime, *m_otTracker,
                                             ambiguity, tu );
        track -> addToMeasurements( otTim );
        nOTMeas++;
        debug() << " - added OTMeasurement, ambiguity = "
                << ambiguity << endreq;
      }
    }
    // next cluster
    ++itOTTime;
  } // loop over outer tracker clusters

  debug() << "- " << nOTMeas << " OTMeasurements added" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addITClusters( MCParticle* mcPart,
                                              Track* track )
{
  unsigned int nITMeas = 0;

  /// Retrieve ITClusters from inverse relation 
  ITCluster2MCParticleAsct::FromRange range = m_itClus2MCP -> rangeTo( mcPart );
  ITCluster2MCParticleAsct::FromIterator iClus;
  for ( iClus = range.begin(); iClus != range.end(); ++iClus) {
    ITCluster* aCluster = iClus->to();
    ITMeasurement meas = ITMeasurement( *aCluster, *m_itTracker );
    track -> addToMeasurements( meas );
    nITMeas++;
  }

  debug() << "- " << nITMeas << " ITMeasurements added" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode IdealTracksCreator::addVeloClusters( MCParticle* mcPart, 
                                                Track* track )
{
  unsigned int nVeloRMeas   = 0;
  unsigned int nVeloPhiMeas = 0;

  /// Retrieve VeloClusters from inverse relation 
  VeloCluster2MCParticleAsct::FromRange range = m_veloClus2MCP->rangeTo(mcPart);
  VeloCluster2MCParticleAsct::FromIterator iClus;
  for ( iClus = range.begin(); iClus != range.end(); ++iClus) {
    VeloCluster* aCluster = iClus->to();
    // Check if velo cluster is r or phi clusters
    if ( m_velo -> isRSensor( aCluster->sensor() ) ) {

      double z = m_velo -> zSensor( aCluster -> sensor() );
      double phi = 999.0;
      State* tempState;
      StatusCode sc = m_stateCreator -> createState( mcPart, z, tempState );
if ( sc.isSuccess() ) {
        HepVector vec = tempState -> stateVector();
        phi = atan2( vec[1], vec[0] );
      }
      delete tempState;

      VeloRMeasurement meas = VeloRMeasurement( *aCluster, *m_velo, phi );
      track -> addToMeasurements( meas );
      nVeloRMeas++;
    }
    else {
      VeloPhiMeasurement meas = VeloPhiMeasurement( *aCluster, *m_velo );
      track -> addToMeasurements( meas );
      nVeloPhiMeas++;
    }
  }

  debug() << "- " << nVeloRMeas << " / " << nVeloPhiMeas
          << " Velo R/Phi Measurements added" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Register the tracks container in the TES. TES becomes owner of the cont.
//=============================================================================
StatusCode IdealTracksCreator::registerTracks( Tracks* tracksCont )
{
  StatusCode sc = put( tracksCont, m_tracksTESPath );
  if ( sc.isFailure() )
    error() << "Unable to register the output container at "
            << m_tracksTESPath << ". Status is " << sc << endreq;
  verbose() << "Tracks container stored in the TES" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Initialize seed state
//=============================================================================
StatusCode IdealTracksCreator::initializeState( double z,
                                                Track* track,
                                                MCParticle* mcPart )
{
  State* state;
  StatusCode sc = m_stateCreator -> createState( mcPart, z, state );
  if ( sc.isSuccess() ) {
    // set covariance matrix to a somewhat larger value for the fit
    HepSymMatrix& cov = state -> covariance();
    cov.fast(1,1) = m_errorX2;
    cov.fast(2,2) = m_errorY2;
    cov.fast(3,3) = m_errorTx2;
    cov.fast(4,4) = m_errorTy2;
    cov.fast(5,5) = pow( m_errorP * state->qOverP(), 2. );

    track -> addToStates( *state );

    double qP = state -> qOverP();

    debug() << "- State added:" << endreq
            << "  - position = " << state -> position() << " mm" <<endreq
            << "  - momentum = " << state -> momentum() << " MeV" <<endreq
            << "  - P        = " << state -> p() << " MeV" <<endreq
            << "  - charge   = " << ( qP != 0. ? int(fabs(qP)/qP) : 0  ) << endreq;
  }
  else {
    delete state;
    debug() << "-> unable to add a State!" << endreq;
  }
  
  return sc;
}

//=============================================================================
// Delete all states on the track
//=============================================================================
StatusCode IdealTracksCreator::deleteStates( Track* track )
{
  std::vector<State*> tmpStates = track -> states();
  for ( std::vector<State*>::const_iterator it  = tmpStates.begin();
        it != tmpStates.end(); it++) {
    track -> removeFromStates( (*it) );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
