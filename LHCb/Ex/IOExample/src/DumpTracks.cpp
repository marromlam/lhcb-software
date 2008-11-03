// $Id: DumpTracks.cpp,v 1.1 2008-11-03 11:07:52 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event Model
#include "Event/Track.h"

// local
#include "DumpTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpTracks
//
// 2004-07-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(DumpTracks)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpTracks::DumpTracks( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "NumberOfObjectsToPrint", m_numObjects = 5 );
  declareProperty( "TracksLocation", m_tracksLocation = LHCb::TrackLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
DumpTracks::~DumpTracks() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpTracks::execute() {

  LHCb::Tracks* tracksCont = get<LHCb::Tracks>( m_tracksLocation );

  info() << "There are " << tracksCont->size() << " tracks in "
         << m_tracksLocation << endmsg;
  
  if( msgLevel( MSG::DEBUG ) ) {
    unsigned int numPrinted = 0;

    LHCb::Tracks::const_iterator iTrack = tracksCont -> begin();
    for ( ; iTrack != tracksCont->end(); ++iTrack ) {
      if( !msgLevel( MSG::VERBOSE ) && ++numPrinted > m_numObjects ) break;
      debug() << *(*iTrack) << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
