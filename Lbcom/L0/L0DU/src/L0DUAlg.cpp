// $Id: L0DUAlg.cpp,v 1.11 2010-01-20 16:30:58 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
//DAQEvent
#include "Event/RawEvent.h"
// local
#include "L0DUAlg.h"

#include "Event/L0DUBase.h"
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUAlg
//
// 2007-10-25 : Olivier Deschamps
//
// Run the L0DU Emulator according to a Trigger Configuration Key (TCK)
// and produce L0DUReport on TES AND/OR rawBank
//
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( L0DUAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUAlg::L0DUAlg( const std::string& name,
            ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator ) 
{    
  declareProperty( "ProcessorDataLocations"  , m_dataLocations   );
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::Calo);
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::Muon);
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::PileUp);
 // configure DAQ
  declareProperty( "BankVersion"             , m_rawVsn   = 2 );
  declareProperty( "EmulatorTool"            , m_emulatorType="L0DUEmulatorTool");
  //
  declareProperty( "RawLocation"             , m_rawLocation    = LHCb::RawEventLocation::Default   );
  declareProperty( "L0DUReportLocation"          , m_reportLocation = LHCb::L0DUReportLocation::Default );
  //
  declareProperty( "TCK"                     , m_tck="");
  declareProperty( "L0DUConfigProviderName"  , m_configName="L0DUConfig");
  declareProperty( "BankSourceID"            ,  m_rawSrcID = 0);

  m_rawBankType  = LHCb::RawBank::L0DU; // rawBank Type


  if( context() == "Emulation" )m_reportLocation = LHCb::L0DUReportLocation::Emulated;
  

}


//=============================================================================
// Destructor
//=============================================================================
L0DUAlg::~L0DUAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0DUAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;


  //---------------------------
  if(m_tck == ""){
    error() << "The Trigger Configuration Key is undefined" << endmsg;
    return StatusCode::FAILURE;
  }
  if( "0x" != m_tck.substr( 0, 2 ) ){
    error() << "The requested TCK value " << m_tck << " MUST be in hexadecimal format '0x" << m_tck << "'" << endmsg;
    return StatusCode::FAILURE;
  }


  //----------------------------
  // Get the L0DUAlg emulator tool
  //---------------------------
  m_emulator = tool<IL0DUEmulatorTool>(m_emulatorType,m_emulatorType,this);
  
  //--------------------------------
  // Get the L0DUAlg configuration tool
  //--------------------------------
  int itck;
  std::istringstream is( m_tck.c_str() );
  is >> std::hex >> itck;
  m_confTool = tool<IL0DUConfigProvider>("L0DUMultiConfigProvider" , m_configName );
  debug() << " loading the configuration for TCK = " << m_tck << " /  " << itck << endmsg;
  m_config   = m_confTool->config( itck );
  if( NULL == m_config){
    error() << " Unable to load the configuration for TCK = " << m_tck << " /  " << itck << endmsg;
    return StatusCode::FAILURE;
  }

  info()<< "The L0DUConfig (TCK=" << m_tck << ") have been succesfully loaded" << endmsg;

  // define standard locations (L0Context-depending)
  m_locs.clear();
  for( std::vector<std::string>::iterator it = m_dataLocations.begin() ; m_dataLocations.end() != it ; ++it){
    std::string loc = dataLocation( *it );
    m_locs.push_back( loc );
  }
    

  return sc;
};


//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUAlg::execute() {


  // process the emulator
  debug() << "Emulator processing ( Data = " << m_locs << ", TCK = " << m_tck << " )" <<endmsg;
  StatusCode sc = m_emulator->process(m_config, m_locs );
  if(sc.isFailure()){
    Error("Cannot process the emulator").ignore();
    return sc;
  }
  
  // push Report and Config on TES
  if(m_writeOnTES){
    debug() <<"Push L0DUReport on TES" << endmsg;
    LHCb::L0DUReport* report = new LHCb::L0DUReport(m_emulator->emulatedReport());
    std::string loc = dataLocation( m_reportLocation );
    put (report   , loc );
  }
  
  //push bank in RawBuffer
  if(m_writeBanks){
    debug() << "Insert RawBank in rawEvent" << endmsg;
    const std::vector<unsigned int> block = m_emulator->bank(m_rawVsn);
    LHCb::RawEvent* raw = getOrCreate<LHCb::RawEvent,LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    raw->addBank(m_rawSrcID , m_rawBankType , m_rawVsn , block);
  } 

  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUAlg::finalize() {
  debug()<< "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}



