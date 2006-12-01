
//-----------------------------------------------------------------------------
/** @file RichRecTimeMonitor.cpp
 *
 *  Implementation file for algorithm class : RichRecTimeMonitor
 *
 *  CVS Log :-
 *  $Id: RichRecTimeMonitor.cpp,v 1.7 2006-12-01 16:34:07 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecTimeMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RichRecTimeMonitor );

// Standard constructor, initializes variables
RichRecTimeMonitor::RichRecTimeMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_nEvents           ( 0 ),
    m_nPIDs             ( 0 ),
    m_totTime           ( 0 )
{

  // Location of RichPIDs in TES
  declareProperty( "RichPIDLocation", m_PIDLocation = RichPIDLocation::Default );

  // Name associated to algorithm(s)
  declareProperty( "TimingName", m_name );

  // List of algorithms
  declareProperty( "Algorithms", m_algNames );

  // Timing boundaries
  declareProperty( "MaxEventTime",       m_maxTime       = 3000 );
  declareProperty( "MaxEventTimePerPID", m_maxTimePerPID = 50   );

}

// Destructor
RichRecTimeMonitor::~RichRecTimeMonitor() {};

//  Initialize
StatusCode RichRecTimeMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // do any init things here ...

  return sc;
}

// Main execution
StatusCode RichRecTimeMonitor::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Acquire PID results
  if ( !loadPIDData() ) return StatusCode::SUCCESS;
  const unsigned nPIDs = m_richPIDs.size();

  // Processing time - sum time for selected algorithms
  double time = 0;
  for ( AlgorithmNames::const_iterator name = m_algNames.begin();
        name != m_algNames.end(); ++name )
  {
    time += chronoSvc()->chronoDelta((*name)+":execute",IChronoStatSvc::ELAPSED)/1000;
  }
  const double timePerPID = ( nPIDs>0 ? time/static_cast<double>(nPIDs) : 0 );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << m_name << " : Time = " << time << " ms for " << nPIDs << " PIDs. "
            << timePerPID << " ms/PID" << endreq;
  }

  // increment counters
  ++m_nEvents;
  m_totTime += time;
  m_nPIDs   += nPIDs;

  // Fill histograms

  const int maxTracks = 150;
  const int maxPixels = 5000;

  plot1D( time, "totTime", m_name+" total processing time (ms)", 0, m_maxTime );
  plot1D( timePerPID, "pidTime", m_name+" processing time per PID (ms)",0,m_maxTimePerPID );

  plot2D( nPIDs, time, "tottimeVnpids", m_name+" total processing time (ms) V #PIDs",
          0.5, maxTracks+0.5, 0, m_maxTime, 1+maxTracks );
  plot2D( richPixels()->size(), time, "tottimeVnpixs", m_name+" total processing time (ms) V #Pixels",
          0.5,maxPixels+0.5,0,m_maxTime );
  plot2D( nPIDs, timePerPID, "pidtimeVnpids", m_name+" processing time per PID (ms) V #PIDs",
          0.5,maxTracks+0.5, 0,m_maxTimePerPID, 1+maxTracks );
  plot2D( richPixels()->size(), timePerPID, "pidtimeVnpixs",
          m_name+" processing time per PID (ms) V #Pixels",0.5,maxPixels+0.5,0,m_maxTimePerPID );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichRecTimeMonitor::finalize()
{
  // Printout timing info
  const double evtTime = ( m_nEvents>0 ? m_totTime/static_cast<double>(m_nEvents) : 0 );
  const double pidTime = ( m_nPIDs>0   ? m_totTime/static_cast<double>(m_nPIDs)   : 0 );
  info() << "Average timing : " << evtTime << " ms/event, "
         << pidTime << " ms/PID" << endreq;

  // return
  return RichRecHistoAlgBase::finalize();
}

StatusCode RichRecTimeMonitor::loadPIDData()
{
  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_PIDLocation, pObject ) )
  {
    if ( KeyedContainer<RichPID, Containers::HashMap> * pids =
         static_cast<KeyedContainer<RichPID, Containers::HashMap>*> (pObject) )
    {
      m_richPIDs.erase( m_richPIDs.begin(), m_richPIDs.end() );
      pids->containedObjects( m_richPIDs );
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  return Warning( "Failed to located RichPIDs at " + m_PIDLocation );
}
