// $Id: RichRecAlgBase.cpp,v 1.4 2003-07-01 16:56:03 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// Event
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

// interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPixelCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecAlgBase
//
// 05/04/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : Algorithm ( name, pSvcLocator ),
    m_procStat(0),
    m_richTracks(0),
    m_richPixels(0),
    m_richSegments(0),
    m_richPhotons(0),
    m_richStatus(0),
    m_trTracks(0),
    m_toolList() {
  
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );
  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "RichRecStatusLocation",
                   m_richRecStatusLocation = RichRecStatusLocation::Default );

}

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize() {

  // Get the current message service printout level
  IntegerProperty msgLevel;
  IProperty* algIProp;
  this->queryInterface( IID_IProperty, (void**)&algIProp );
  msgLevel.assign( algIProp->getProperty( "OutputLevel" ) );
  m_msgLevel = msgLevel;
  algIProp->release();

  // Get pointer to Rich Tool Registry
  if ( !toolSvc()->retrieveTool( "RichToolRegistry", m_toolReg) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "RichToolRegistry not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  // initialise data pointers
  IRichSegmentCreator * segTool; acquireTool("RichSegmentCreator", segTool);
  m_richSegments = &(segTool->richSegments());
  IRichPhotonCreator * photTool; acquireTool("RichPhotonCreator", photTool);
  m_richPhotons = &(photTool->richPhotons());
  IRichPixelCreator * pixTool; acquireTool("RichPixelCreator", pixTool);
  m_richPixels = &(pixTool->richPixels());
  IRichTrackCreator * tkTool; acquireTool("RichTrackCreator", tkTool);
  m_richTracks = &(tkTool->richTracks());

  return StatusCode::SUCCESS;
};

TrStoredTracks * RichRecAlgBase::updateTrTracks() {

  // Obtain smart data pointer to TrStoredTracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trTracksLocation );
  if ( !tracks ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to locate TrStoredTracks at "
        << m_trTracksLocation << endreq;
  }

  return (m_trTracks = tracks);
}

RichRecStatus * RichRecAlgBase::updateRichStatus() {

  SmartDataPtr<RichRecStatus> status( eventSvc(), m_richRecStatusLocation );
  if ( status ) {
    m_richStatus = status;
  } else {
    m_richStatus = new RichRecStatus();
    if ( !eventSvc()->registerObject(m_richRecStatusLocation, m_richStatus) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::ERROR << "Failed to register RichRecStatus at "
          << m_richRecStatusLocation << endreq;
      delete m_richStatus;
      m_richStatus = NULL;
    }
  }

  return m_richStatus;
}

ProcStatus * RichRecAlgBase::updateProcStatus()  {
  
  SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
  if ( !procStat ) {
    MsgStream msg ( msgSvc(), name() );
    msg << MSG::WARNING << "Failed to locate ProcStatus at " 
        << m_procStatLocation << endreq;
  }

  return ( m_procStat = procStat );
}

//  Finalize
StatusCode RichRecAlgBase::finalize() {

  // Release all tools
  for ( ToolList::iterator it = m_toolList.begin();
        it != m_toolList.end(); ++it ) {
    if ( (*it).second ) { releaseTool((*it).first); }
  }
  if ( m_toolReg ) { toolSvc()->releaseTool( m_toolReg ); m_toolReg = NULL; }

  return StatusCode::SUCCESS;
}
