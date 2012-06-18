// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/AllLinks.h"

#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"
#include "Event/MCProperty.h"
// local
#include "PrChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrChecker
//
// 2005-03-29 : Olivier Callot
//modified by Wenbin Qian for Upgrade
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( PrChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrChecker::PrChecker( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "VeloTracks",        m_veloTracks      = LHCb::TrackLocation::Velo    );
  declareProperty( "ForwardTracks",     m_forwardTracks   = LHCb::TrackLocation::Forward );
  declareProperty( "SeedTracks",        m_seedTracks      = LHCb::TrackLocation::Seed    );
  
}
//=============================================================================
// Destructor
//=============================================================================
PrChecker::~PrChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_velo = tool<PrCounter>( "PrCounter", "Velo", this );
  m_velo->setContainer( m_veloTracks );
  m_velo->setSelectId( 3 );
  m_velo->addSelection( "OK for Velo" );
  m_velo->addSelection( "long" );
  m_velo->addSelection( "long > 5 GeV" );
  m_velo->addSelection( "Strange+long" );
  m_velo->addSelection( "and > 5 GeV" );
  m_velo->addSelection( "long from B decay" );
  m_velo->addSelection( "and > 5 GeV" );

  m_forward = tool<PrCounter>( "PrCounter", "Forward", this );
  m_forward->setContainer( m_forwardTracks );
  m_forward->setSelectId( 8 );
  m_forward->addSelection( "long" );
  m_forward->addSelection( "long > 5 GeV" );
  m_forward->addSelection( "Strange+long" );
  m_forward->addSelection( "and > 5 GeV" );
  m_forward->addSelection( "long from B decay" );
  m_forward->addSelection( "and > 5 GeV" );

  m_tTrack = tool<PrCounter>( "PrCounter", "TTrack", this );
  m_tTrack->setContainer( m_seedTracks );
  m_tTrack->setSelectId( 8 );
  m_tTrack->addSelection( "has T hits" );
  m_tTrack->addSelection( "long" );
  m_tTrack->addSelection( "long > 5 GeV" );
  m_tTrack->addSelection( "Strange+T+TT" );
  m_tTrack->addSelection( "and > 5 GeV" );

  m_best = tool<PrCounter>( "PrCounter", "Best", this );
  m_best->setContainer( LHCb::TrackLocation::Default );
  m_best->setSelectId( 15 );
  m_best->addSelection( "long" );
  m_best->addSelection( "long > 5 GeV" );
  m_best->addSelection( "Strange+long" );
  m_best->addSelection( "and > 5 GeV" );
  m_best->addSelection( "long from B decay" );
  m_best->addSelection( "and > 5 GeV" );

  m_allCounters.push_back( m_velo  );
  m_allCounters.push_back( m_forward );
  m_allCounters.push_back( m_tTrack );
  m_allCounters.push_back( m_best  );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrChecker::execute() {

  debug() << "==> Execute" << endmsg;

  for ( std::vector<PrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->initEvent();
  }


  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  //== Build a table (vector of vectors) of ids per MCParticle, indexed by MCParticle key.
  AllLinks<LHCb::MCParticle> allIds( evtSvc(), msgSvc(), "Pr/LHCbID" );
  std::vector< std::vector<int> > linkedIds;
  LHCb::MCParticle* part = allIds.first();
  while ( NULL != part ) {
    unsigned int minSize = part->key();
    while ( linkedIds.size() <= minSize ) {
      std::vector<int> dum;
      linkedIds.push_back( dum );
    }
    linkedIds[part->key()].push_back( allIds.key() );
    part = allIds.next();
  }

  LHCb::MCParticles::const_iterator itP;
  for ( itP = mcParts->begin(); mcParts->end() != itP; ++itP ) {
    LHCb::MCParticle* part = *itP;
    if ( 0 == trackInfo.fullInfo( part ) ) continue;
    verbose() << "checking MCPart " << part->key() << endreq;

    bool isLong  = trackInfo.hasVeloAndT( part );
    isLong = isLong && ( abs( part->particleID().pid() ) != 11 ); // and not electron
    
    bool over5       = 5000. < fabs( part->p() );
    bool isInVelo    = trackInfo.hasVelo( part );
    bool strangeLong = false;
    bool strangeDown = false;
    bool fromB       = false;
    
    if ( 0 != part->originVertex() ) {
      const LHCb::MCParticle* mother =  part->originVertex()->mother();
      if ( 0 != mother ) {
        if ( 0 != mother->originVertex() ) {
          double rOrigin = mother->originVertex()->position().rho();
          if ( fabs( rOrigin ) < 5. ) {
            int pid = abs( mother->particleID().pid() );
            if ( 130 == pid ||    // K0L
                 310 == pid ||    // K0S
                 3122 == pid ||   // Lambda
                 3222 == pid ||   // Sigma+
                 3212 == pid ||   // Sigma0
                 3112 == pid ||   // Sigma-
                 3322 == pid ||   // Xsi0
                 3312 == pid ||   // Xsi-
                 3334 == pid      // Omega-
                 ) {
              strangeDown = trackInfo.hasT( part ) &&  trackInfo.hasTT( part );
              strangeLong = trackInfo.hasVeloAndT( part );
            }
          }
        }
      }
      while( 0 != mother ) {
        if ( mother->particleID().hasBottom() ) fromB = true;
        mother = mother->originVertex()->mother();
      }
    }
    
    std::vector<LHCb::LHCbID> ids;
    if ( linkedIds.size() > (unsigned int) part->key() ) {
      for ( std::vector<int>::const_iterator itIm = linkedIds[part->key()].begin();
            linkedIds[part->key()].end() != itIm; ++itIm ) {
        LHCb::LHCbID temp;
        temp.setDetectorType( (*itIm) >> 28 );  // create LHCbId from int. Clumsy !
        temp.setID( (*itIm) );
        ids.push_back( temp );
      }
    }
    
    verbose() << "MCPart " << part->key() << " has " << ids.size() << " LHCbIDs " <<endmsg;
    
    std::vector<bool> flags;
    flags.push_back( isInVelo );
    flags.push_back( isLong  );
    flags.push_back( isLong && over5   );
    flags.push_back( isLong && strangeLong );
    flags.push_back( isLong && strangeLong && over5 );
    flags.push_back( isLong && fromB );
    flags.push_back( isLong && fromB && over5 );

    m_velo->count( part, flags, ids );

    flags.clear();
    flags.push_back( isLong  );
    flags.push_back( isLong && over5   );
    flags.push_back( isLong && strangeLong );
    flags.push_back( isLong && strangeLong && over5 );
    flags.push_back( isLong && fromB );
    flags.push_back( isLong && fromB && over5 );

    m_forward->count( part, flags, ids );
    m_best->count( part, flags, ids );

    flags.clear();
    flags.push_back( trackInfo.hasT( part ) );
    flags.push_back( isLong );
    flags.push_back( isLong && over5 );
    flags.push_back( strangeDown );
    flags.push_back( strangeDown && over5 );

    m_tTrack->count( part, flags, ids );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  for ( std::vector<PrCounter*>::iterator itC = m_allCounters.begin();
        m_allCounters.end() != itC; ++itC ) {
    (*itC)->printStatistics();
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

