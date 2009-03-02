// $Id: DaVinciInit.cpp,v 1.1 2009-03-02 21:10:02 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Incident.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/Memory.h"

// from EventBase
#include "Event/ProcessHeader.h"

#include "Event/ODIN.h"

// from RecEvent
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"

// local
#include "DaVinciInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciInit
//
// 2008-03-02 : P. Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DaVinciInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinciInit::DaVinciInit( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator )
    , m_lastMem(0)
{
  declareProperty("PrintEvent", m_print =  false, "Print Event and Run Number");
  declareProperty("Increment", m_increment = 100, "Number of events to measure memory");
  
}
//=============================================================================
// Destructor
//=============================================================================
DaVinciInit::~DaVinciInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DaVinciInit::initialize() {
  StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  m_memoryTool = tool<IGenericTool>( "MemoryTool", "DaVinciMemory", this, true );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DaVinciInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit
  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the ODIN bank
  if ( m_print ){
    if ( exist<LHCb::ODIN> ( LHCb::ODINLocation::Default )){
      const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
      unsigned int runNumber = odin->runNumber();
      ulonglong    evtNumber = odin->eventNumber();
      this->printEventRun( evtNumber, runNumber );
    } else Warning("No ODIN bank",StatusCode::SUCCESS,1);
  }
  
  counter("Events")++ ;
  unsigned int nev = counter("Events").nEntries() ;
  int mem = System::virtualMemory();

  if (msgLevel(MSG::DEBUG)) debug() << nev << " memory: " << mem << " KB" << endmsg ;
  
  if ( 0 == m_lastMem ) m_lastMem = mem ;
  else if ( 0 == nev%m_increment) {
    if ( m_lastMem < mem ){
      info() << "Memory has increased from " << m_lastMem << " to " << mem << "  KB" 
             << " (+" << 100.*(mem-m_lastMem)/mem << "%)" 
             << " in last " << m_increment << " events" << endmsg ;
    }
    m_lastMem = mem ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
