// $Id: OTRandomDepositCreator.cpp,v 1.5 2004-12-10 09:54:08 cattanem Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IService.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// OTEvent
#include "Event/MCOTDeposit.h"

// OTSimulation
#include "OTSimulation/IOTReadOutWindow.h"

// OTDet
#include "OTDet/DeOTDetector.h"

// local
#include "OTRandomDepositCreator.h"

/** @file OTRandomDepositCreator.cpp 
 *
 *  Implementation of OT tool.
 *  
 *  @author M. Needham
 *  @date   28/02/2003
 */

static ToolFactory<OTRandomDepositCreator> s_factory;
const IToolFactory& OTRandomDepositCreatorFactory = s_factory;

OTRandomDepositCreator::OTRandomDepositCreator(const std::string& type, 
                     const std::string& name, 
                     const IInterface* parent) : 
  GaudiTool( type, name, parent )
{
 
  this->declareProperty("deadTime", m_deadTime = 50.*ns);
  this->declareProperty("noiseRate", m_noiseRate = 10.0*kilohertz);
  this->declareProperty("readOutWindowToolName",
                        m_readoutWindowToolName ="OTReadOutWindow"),

  declareInterface<IOTRandomDepositCreator>(this);
}

OTRandomDepositCreator::~OTRandomDepositCreator()
{
  //destructor
}

StatusCode OTRandomDepositCreator::initialize() 
{
  StatusCode sc = GaudiTool::initialize();

  // retrieve pointer to random number service
  IRndmGenSvc* randSvc = 0;
  sc = serviceLocator()->service( "RndmGenSvc", randSvc, true ); 
  if( sc.isFailure() ) {
    return Error ("Failed to retrieve random number service",sc);
  }  
  // get interface to generator
  sc = randSvc->generator(Rndm::Flat(0.,1.0),m_genDist.pRef()); 
  if( sc.isFailure() ) {
    return Error ("Failed to generate random number distribution",sc);
  }
  randSvc->release();

  // Loading OT Geometry from XML
  IDataProviderSvc* detSvc; 
  sc = serviceLocator()->service( "DetectorDataSvc", detSvc, true );
  if( sc.isFailure() ) {
    return Error ("Failed to retrieve magnetic field service",sc);
  }

  DeOTDetector* tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default ); 
  detSvc->release();
  m_tracker = tracker;

  // pointer to OTReadoutWindow tool
  IOTReadOutWindow* readoutTool;
  readoutTool = tool<IOTReadOutWindow>(m_readoutWindowToolName);
 
  // calculate window start and size
  std::vector<double> startGates = readoutTool->startReadOutGate();
  std::vector<double>::iterator iterG = startGates.begin();
  while (iterG !=  startGates.end()){
    m_windowOffSet.push_back(*iterG - m_deadTime);
    ++iterG;
  } // iterG

  m_windowSize = readoutTool->sizeOfReadOutGate() + m_deadTime;


  // release tool 
  release(readoutTool);

  m_nMaxChanInModule = m_tracker->nMaxChanInModule();
  m_nNoise = this->nNoiseHits();

  return StatusCode::SUCCESS;  
}

StatusCode OTRandomDepositCreator::createDeposits(MCOTDepositStdVector* depVector)
  const 
{
  // get number of modules 
  std::vector<DeOTModule*> otModules =  m_tracker->modules();
  unsigned int nModules =  otModules.size();
  
  for (unsigned int iDep = 0; iDep < m_nNoise; ++iDep){
   
    unsigned int moduleNum = (unsigned int) (nModules*m_genDist->shoot());
    DeOTModule* aModule = otModules[moduleNum];
    
    unsigned int strawID = (unsigned int)(m_nMaxChanInModule*
                                          m_genDist->shoot())+1u;

    if (strawID <= aModule->nChannels()) {
      unsigned int stationID = aModule->stationID();
      double tdcTime = m_windowOffSet[stationID-1u] 
        + (m_genDist->shoot() * m_windowSize);  
      OTChannelID aChan(stationID, aModule->layerID(), aModule->quarterID(),
                        aModule->moduleID(), strawID);
      MCOTDeposit* newDeposit = new MCOTDeposit(0, aChan, tdcTime, 0, 0);
      depVector->push_back(newDeposit);
    }
  } // iDep

  return StatusCode::SUCCESS;  
}

unsigned int OTRandomDepositCreator::nNoiseHits() const
{ 
  // number of hits to generate
  return ((unsigned int)(this->nChannels()*m_windowSize*m_noiseRate));
}

unsigned int OTRandomDepositCreator::nChannels() const 
{
  // get the maximum number of channels
  std::vector<DeOTModule*> otModules =  m_tracker->modules();
  unsigned int nModules =  otModules.size();
  unsigned int nChan = m_nMaxChanInModule * nModules;

  return nChan;
}
