//      ====================================================================
//      UpdateAndReset.cpp
//      --------------------------------------------------------------------
//
//      Package   : Gaucho
//
//      Author    : Eric van Herwijnen
//      Should be run as first algorithm in TopAlg
//      Checks:
//      1) If the run number has changed. If so, updateAll(true) and resetHistos
//      2) If timerelapsed flag is true. If so, updateAll(false)   
//
//      ====================================================================


// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram.h"
#include "AIDA/IProfile1D.h"
#include "TFile.h"
#include "TH1.h"
#include "TProfile.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TSystem.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "Gaucho/Misc.h"

#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "RTL/rtl.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MDF/OnlineRunInfo.h"


#include "UpdateAndReset.h"
#include "MonitorSvc.h"
#include "RTL/rtl.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(UpdateAndReset)

// Constructor
//------------------------------------------------------------------------------
UpdateAndReset::UpdateAndReset(const std::string& name, ISvcLocator* ploc)
  : GaudiAlgorithm(name, ploc)
{
  declareProperty("disableMonRate", m_disableMonRate = 0);
  declareProperty("desiredDeltaTCycle", m_desiredDeltaTCycle = 20);
  declareProperty("disableReadOdin", m_disableReadOdin = 0);
  declareProperty("disableChekInTimer", m_disableChekInTimer = 0);
  declareProperty("disableChekInExecute", m_disableChekInExecute = 0);
  declareProperty("disableUpdateData", m_disableUpdateData = 0);
  declareProperty("disableResetHistos", m_disableResetHistos = 0);
  
  declareProperty("saveHistograms", m_saveHistograms = 0);
  declareProperty("saveSetDir", m_saveSetDir = "/hist/Savesets");
  declareProperty("saverCycle", m_saverCycle = 900);
  declareProperty("resetHistosAfterSave", m_resetHistosAfterSave = 0);
  
  declareProperty("teste", m_teste = 100000);
  
  m_timerCycle = m_desiredDeltaTCycle - 1;
  m_firstExecute = true;
  m_dimSvcSaveSetLoc = 0;
  
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::initialize() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isSuccess() ) msg << MSG::DEBUG << "GaudiAlgorithm Initialized" << endreq;
  else {
    msg << MSG::FATAL << "GaudiAlgorithm not Initialized" << endreq;
    return StatusCode::FAILURE;
  }

  //const std::string& utgid = RTL::processName();
  m_utgid = RTL::processName();
    
  sc = serviceLocator()->service("MonitorSvc", m_pGauchoMonitorSvc, false);
  if( sc.isSuccess() ) msg << MSG::DEBUG << "Found the IGauchoMonitorSvc interface" << endreq;
  else {
    msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
    return StatusCode::FAILURE;
  }
  // to make the ODIN object. doesn't work.
  //updMgrSvc();
  
  serviceParts = Misc::splitString(m_utgid, "_");
  
  taskName = "unknownTask";
  partName = "unknownPartition";
  instancenumber = "1";
  
  if (3 == serviceParts.size()) {
    partName = serviceParts[0];
    taskName = serviceParts[1];
    instancenumber = serviceParts[2];
    if (partName=="CALD0701") {
       partName="LHCb";
       if (instancenumber=="1") {taskName = "CaloDAQCalib";}
       else {taskName = "Calib"+taskName+"_"+instancenumber;}
    }   
  }
  else if (4 == serviceParts.size()) {
    partName = serviceParts[0];
    taskName = serviceParts[2];
  }
  
  m_infoFileStatus = "SAVESETLOCATION/......................................................";
  infoName = partName+"/"+taskName+"/SAVESETLOCATION";
  if (m_saveHistograms) m_dimSvcSaveSetLoc = new DimService(infoName.c_str(),(char*)m_infoFileStatus.c_str());


  sc = serviceLocator()->service("HistogramDataSvc", m_histogramSvc, true); 
  
  if (sc.isFailure()) msg << MSG::FATAL << "Unable to locate the HistogramDataService" << endreq;
  else msg << MSG::DEBUG << "Found the HistogramDataService" << endreq;
  
  if (0 == m_desiredDeltaTCycle){
//    msg << MSG::WARNING << "Your algorithm is using the UpdateAndReset algrithm which update and reset data every desiredDeltaTCycle seconds. You didn't fill the desiredDeltaTCycle option in your options file, then we will consider 10 seconds as default." << endreq;
    m_desiredDeltaTCycle = 20;
  }
    
  if (1 == m_saveHistograms){
    div_t divresult = div (m_saverCycle, m_desiredDeltaTCycle);
    m_numCyclesToSave = divresult.quot;
    if (0 == divresult.rem ) {
    } 
    else {
      m_saverCycle = divresult.quot * m_desiredDeltaTCycle;
    } 
  }
  
  // The below part is for test
  m_countExecutes = 0; 
  m_deltaTRunTest = 8 * m_desiredDeltaTCycle;

  m_timeStart = GauchoTimer::currentTime();

 // m_runNumber = currentRunNumber().first.first;
  m_triggerConfigurationKey = currentTCK();
  m_cycleNumber = currentCycleNumber(m_timeStart).first;
  m_timeFirstEvInRun = m_timeStart;
  m_offsetTimeFirstEvInRun = offsetToBoundary(m_cycleNumber, m_timeFirstEvInRun, true);
  m_timeLastEvInCycle = m_timeStart;
  m_gpsTimeLastEvInCycle = gpsTime();
  m_runStatus = s_statusNoUpdated;
  m_cycleStatus = s_statusNoUpdated;

  if (0==m_disableMonRate)  m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumber, m_triggerConfigurationKey, m_cycleNumber, m_deltaTCycle, m_offsetTimeFirstEvInRun, m_offsetTimeLastEvInCycle, m_offsetGpsTimeLastEvInCycle);
   return StatusCode::SUCCESS;
}

void UpdateAndReset::timerHandler()
{
  MsgStream msg(msgSvc(), name());
  if (0 != m_disableChekInTimer) {
    return;
  }

  verifyAndProcessCycleChange(true);

  DimTimer::start(m_timerCycle); // we verify the cycle status every 9 seconds
}

StatusCode UpdateAndReset::testeExecute() {
  MsgStream msg( msgSvc(), name() );
  
/*  div_t divresult = div (m_countExecutes,1000);
  if (divresult.rem == 0) msg << MSG::
  verifyAndProcessCycleChange(true);

  DimTimer::start(m_timerCycle); // we verify the cycle status every 9 seconds << "Execute method # " << m_countExecutes << endreq;*/
  //msg << MSG::INFO << "Execute method # " << m_countExecutes << endreq;
  if (m_countExecutes == m_teste) {
   // msg << MSG::INFO << "Execute method # " << m_countExecutes << endreq;
    //m_pGauchoMonitorSvc->resetHistos(true);
    m_countExecutes = 0;
  }
  m_countExecutes++;
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

  m_countExecutes++;

  if (0 == m_disableChekInExecute){
    m_triggerConfigurationKey = currentTCK();
    verifyAndProcessCycleChange(false);
    verifyAndProcessRunChange();
  }

  // Because the plot method we start the timer after the first execute...
  // This is because of plot method declareinfo in the execute method...
  if (m_firstExecute){
    m_firstExecute = false;
    DimTimer::start(m_timerCycle);
    m_firstCycleNumber = currentCycleNumber(GauchoTimer::currentTime()).first;
    //only do this for the first event, so we have a runnumber
    m_runNumber = currentRunNumber().first.first;
   // msg << MSG::INFO << "Trigger Configuration Key " << m_triggerConfigurationKey << endreq;
  }
  return StatusCode::SUCCESS;
}

void UpdateAndReset::verifyAndProcessCycleChange(bool isFromTimerHandler) {
  ulonglong currentTime = GauchoTimer::currentTime();
  std::pair<int, bool> cycleNumber = currentCycleNumber(currentTime);
  if (!cycleNumber.second) return; // false means that the cycleNumber wasn't change
  if (s_statusNoUpdated != m_cycleStatus) return; // We update if the cycleStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  std::string method = "execute";
  if (isFromTimerHandler) method = "timerHandler";
  m_cycleStatus = s_statusProcessingUpdate;
 // msg << MSG::INFO<< " updating (verifyandprocesscyclechange)" << endreq;  
  updateData(false, isFromTimerHandler); //1er param false means that the update wasn't called when runNumber changed
  m_cycleStatus = s_statusUpdated;
  retrieveCycleNumber(cycleNumber.first);
}

void UpdateAndReset::verifyAndProcessRunChange() { // this method can not be called from TimerHandler
  std::pair<std::pair<int, ulonglong>, bool> runNumber = currentRunNumber(); // if this is too late, we can not avoid two process calling it.
  MsgStream msg(msgSvc(), name());
 // msg << MSG::INFO<< " runnumber " << runNumber.first.first << " gps time " << runNumber.first.second << " runNumber.second " << runNumber.second << endreq;
  if (!runNumber.second) return;// false means that the runNumber wasn't changed
  if (s_statusNoUpdated != m_runStatus) return; // We update if the runStatus is different of ProcessingUpdate and Updated
  //stop the dimtimer
  DimTimer::stop();
  //msg << MSG::INFO << " The Run Number has changed then we UpdateAll and reset Histograms " << runNumber.first.first << endreq;
  m_runStatus = s_statusProcessingUpdate;
  //don't update the first time
  if (runNumber.first.first!=0) updateData(true, false); //1er param true means that the update was called because the runNumber changed and 2d false means no TimerHandler
  m_runStatus = s_statusUpdated;
  retrieveRunNumber(runNumber.first.first, runNumber.first.second);
  m_triggerConfigurationKey=currentTCK();
  //restart the DimTimer
  DimTimer::start(m_timerCycle);
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
//  msg << MSG::DEBUG << "finalizing...." << endreq;
  if ( 1 == m_saveHistograms ) {
     //calling finalize - don't need to reset, they probably don't exist anymore
     m_eorNumber=m_runNumber;
     manageTESHistos(false, false, true, true);
  }
  else {
    updateData(false,false);
  }
  DimTimer::stop();

  if (m_dimSvcSaveSetLoc !=0 ) {delete m_dimSvcSaveSetLoc; m_dimSvcSaveSetLoc=0;}

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveRunNumber(int runNumber, ulonglong gpsTime) {
//------------------------------------------------------------------------------

  m_runNumber = runNumber;  
  m_runStatus = s_statusNoUpdated;
  m_timeFirstEvInRun = gpsTime;
  m_offsetTimeFirstEvInRun = offsetToBoundary(currentCycleNumber(m_timeFirstEvInRun).first, m_timeFirstEvInRun, true);
}

//------------------------------------------------------------------------------
// std::pair<int, bool> UpdateAndReset::currentRunNumber() {
std::pair<std::pair<int, ulonglong>, bool> UpdateAndReset::currentRunNumber() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  int runNumber=0;
  gpstime = GauchoTimer::currentTime();
  bool changed = false;

  //msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if( exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default ) ) {
       m_rawEvt= get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
       LHCb::RawBank::BankType i = LHCb::RawBank::BankType(16);
       std::string bname = LHCb::RawBank::typeName(i);       
       std::string::size_type odinfound=bname.find("ODIN",0);

       if (odinfound!=std::string::npos) {
          const std::vector<LHCb::RawBank*>& b =m_rawEvt->banks(i);
          if ( b.size() > 0 )  {
            std::vector<LHCb::RawBank*>::const_iterator itB;
           // int k = 0;
            for( itB = b.begin(); itB != b.end(); itB++ ) {
               //if ( ((k++)%4) == 0 ) info() << endmsg << "  " ;
               const LHCb::RawBank* r = *itB;
	      // const LHCb::OnlineRunInfo* ori = 0;
	       const LHCb::OnlineRunInfo* ori=r->begin<LHCb::OnlineRunInfo>();
	       tck=ori->TCK;
	       gpstime=ori->GPSTime;
	       runNumber=ori->Run;		 
           }
        }
      }
      else {
         msg << MSG::DEBUG << "ODIN bank not found at location '" << rootInTES() << LHCb::RawEventLocation::Default << endreq;;      
      }
 
    //  msg << MSG::INFO  << "runnumber " << runNumber << " ctck " << ctck << " tck " << tck << " cgpstime " <<
    //  cgpstime << " gpstime " << gpstime << endmsg;

    }

    else  
    {
       msg << MSG::DEBUG << "rawEvent not found at location '" << rootInTES() << LHCb::RawEventLocation::Default << endreq;
    }	
  }
  else {       
//    msg << MSG::DEBUG<< "===============> Reading Odin bank is disabled. Then runNumber = 0 and gpsTime = currentTime" <<endreq;
  }
  if ((m_runNumber != runNumber) && (m_runNumber !=0)) {
     changed = true;
     m_eorNumber = m_runNumber;
  }  
  std::pair<int, ulonglong> runNumberGpsTime = std::pair<int, ulonglong>(runNumber, gpstime);
//  msg << MSG::DEBUG << "old runnumber " << m_runNumber << " new runnumber " << runNumber << " changed " << changed << endreq; 
  m_runNumber = runNumber;
  
  // return std::pair<int, bool>(runNumber,changed);
  return std::pair<std::pair<int, ulonglong>, bool>(runNumberGpsTime, changed);
}
//------------------------------------------------------------------------------

unsigned int UpdateAndReset::currentTCK() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  unsigned int triggerConfigurationKey=0;

  //msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    triggerConfigurationKey=tck;
  }
  else {
  }
  return triggerConfigurationKey;
}

//------------------------------------------------------------------------------
ulonglong UpdateAndReset::gpsTime() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

//  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {

    return gpstime;
  }
//  else msg << MSG::DEBUG<< "===============> Reading Odin bank is disabled. " <<endreq;

  // this is only for test when Odin doesn't work
  ulonglong currentTime = GauchoTimer::currentTime();

  int cycleNumber = (int)currentTime/(m_desiredDeltaTCycle*1000000);

  gpstime = ((ulonglong)cycleNumber)*((ulonglong) m_desiredDeltaTCycle*1000000);

  return gpstime;
}

std::pair<int, bool> UpdateAndReset::currentCycleNumber(ulonglong currentTime) {
  bool changed = false;
  int cycleNumber = (int)(((double)currentTime)/(m_desiredDeltaTCycle*1000000.00));
  
  if (m_cycleNumber != cycleNumber ) changed = true;
  return std::pair<int, bool>(cycleNumber,changed);
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveCycleNumber(int cycleNumber) {
//------------------------------------------------------------------------------
  // In order to avoid conflict, the cycle number is updated here and only here
  m_cycleNumber = cycleNumber;
  m_cycleStatus = s_statusNoUpdated;
}

double UpdateAndReset::offsetToBoundary(int cycleNumber, ulonglong time, bool inferior){
  if (inferior) {
    ulonglong timeIniCycle = ((ulonglong) cycleNumber)*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeIniCycle));
  }
  else {
    ulonglong timeEndCycle = ((ulonglong) (cycleNumber+1))*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeEndCycle));
  }
}

void UpdateAndReset::updateData(bool isRunNumberChanged, bool isFromTimerHandler) {
  MsgStream msg( msgSvc(), name() );
  ulonglong currentTime = GauchoTimer::currentTime();
//  msg << MSG::DEBUG << "**********************************************************************" << endreq;
//  msg << MSG::INFO << "************Updating data " << (currentTime - m_timeStart) << " microseconds after start **********" << endreq;  msg << MSG::DEBUG << "m_runNumber        = " << m_runNumber << endreq;
//  msg << MSG::INFO << "m_cycleNumber      = " << m_cycleNumber << endreq;
//  msg << MSG::DEBUG << "m_timeFirstEvInRun      = " << m_timeFirstEvInRun << endreq;
//  msg << MSG::DEBUG << "m_offsetTimeFirstEvInRun      = " << m_offsetTimeFirstEvInRun << endreq;
  m_deltaTCycle = (double)currentTime - (double)m_timeLastEvInCycle;
//  msg << MSG::DEBUG << "m_deltaTCycle = " << m_deltaTCycle << " microseconds" << endreq;
  m_timeLastEvInCycle = currentTime;
  m_offsetTimeLastEvInCycle = offsetToBoundary(m_cycleNumber, m_timeLastEvInCycle, false);
//  msg << MSG::DEBUG << "m_timeLastEvInCycle     = " << m_timeLastEvInCycle << endreq;
//  msg << MSG::DEBUG << "m_offsetTimeLastEvInCycle     = " << m_offsetTimeLastEvInCycle << endreq;
//  msg << MSG::DEBUG << "deltaT error = " << m_deltaTCycle - m_desiredDeltaTCycle*1000000 << " microseconds" << endreq;
  
  if (isFromTimerHandler) m_gpsTimeLastEvInCycle = currentTime; // we can not read ODIN from timerHandler
  else  m_gpsTimeLastEvInCycle = gpsTime();
    
  m_offsetGpsTimeLastEvInCycle = offsetToBoundary(m_cycleNumber, m_gpsTimeLastEvInCycle, false);
  
//  msg << MSG::DEBUG << "m_gpsTimeLastEvInCycle  = " << m_gpsTimeLastEvInCycle << endreq;
//  msg << MSG::DEBUG << "m_offsetGpsTimeLastEvInCycle  = " << m_offsetGpsTimeLastEvInCycle << endreq;
//  msg << MSG::INFO << "TimeLastEvent error = " << (m_timeLastEvInCycle - m_gpsTimeLastEvInCycle) << " microseconds runnumberchanged " << isRunNumberChanged << " disablupdatedata " << m_disableUpdateData << endreq;
 
  if (isRunNumberChanged) {
    if (0 == m_disableUpdateData) {
        // msg << MSG::INFO << "updating dim services (runnumber changed)" << endreq;    
       m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
    }
 //   else msg << MSG::DEBUG << "===============> Data was not updated because the UpdateData process is disabled." << endreq;
    if (0 == m_disableResetHistos) {
      if ( 1 == m_saveHistograms ) {
    //     msg << MSG::INFO << "==============> SAVING HISTOS BECAUSE FAST RUN CHANGE<=======================" << endreq;
         manageTESHistos(false, true, true, true);
      }
      else manageTESHistos(false, true, false, true);
    }
 //   else msg << MSG::DEBUG << "===============> resetHistos disabled." << endreq;
  }
  else{
    if (0 == m_disableUpdateData) {      
        m_pGauchoMonitorSvc->updateAll(false);
	}
    if ( 1 == m_saveHistograms ) {
      bool resetHistos = false;
      if(1 == m_resetHistosAfterSave) resetHistos = true;
      if (isSaveCycle(m_cycleNumber)) {
 //       msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE CYCLE SAVER <=======================" << endreq;
        manageTESHistos(false, resetHistos, true, false);
      }
    }
  }
//  msg << MSG::DEBUG << "***************************  End updateData  *************************" << endreq;
//  msg << MSG::DEBUG << "**********************************************************************" << endreq;
}

bool UpdateAndReset::isSaveCycle(int m_cycleNumber) {
  if (m_cycleNumber == m_firstCycleNumber) return false;
  div_t divresult = div (m_cycleNumber - m_firstCycleNumber, m_numCyclesToSave);
  if (0 != divresult.rem) return false;
  return true;
}

void UpdateAndReset::manageTESHistos (bool list, bool reset, bool save, bool isFromEndOfRun) {
  MsgStream msg( msgSvc(), name() );
  IRegistry* object = rootObject();
  int level = 0;
  std::vector<std::string> idList;
  msg << MSG::DEBUG << "managing histos list " << list << " reset " << reset << " save " << save << " endofrun " << isFromEndOfRun << endreq;
  TFile *f=0;
  m_infoFileStatus = "......this is the file name were we will save histograms...........";
  char timestr[64];
  char year[5];
  char month[3];
  char day[3];
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);
  ::strftime(year, sizeof(year),"%Y", timeInfo);
  ::strftime(month, sizeof(month),"%m", timeInfo);  
  ::strftime(day, sizeof(day),"%d", timeInfo);  


  if (save)  {
     //std::string dirName = m_saveSetDir + "/" + year + "/" + partName + "/" + taskName;  
     //add the month and day to avoid too many files per year
     std::string dirName = m_saveSetDir + "/" + year + "/" + partName + "/" + taskName + "/" + month + "/" + day;  
     void *dir = gSystem->OpenDirectory(dirName.c_str());
     if ((dir == 0) && (save)) {
     gSystem->mkdir(dirName.c_str(),true);
    }
    std::string tmpfile="";
    //add runnumber to saveset name
    
    if (m_runNumber != 0) {
       std::string runNumberstr;
       std::stringstream outstr;
       if (isFromEndOfRun) outstr << m_eorNumber;
       else outstr << m_runNumber;
       runNumberstr=outstr.str();    
       if (isFromEndOfRun) tmpfile = dirName + "/" + taskName + "-" + runNumberstr + "-" + timestr + "-EOR.root"; 
       else tmpfile = dirName + "/" + taskName + "-" + runNumberstr + "-" + timestr + ".root"; }
    else { 
       if (isFromEndOfRun)  tmpfile = dirName + "/" + taskName + "-" + timestr + "-EOR.root";
       else tmpfile = dirName + "/" + taskName + "-" + timestr + ".root"; 
    }	 
   // std::string tmpfile = dirName + "/" + taskName + "-" + timestr + ".root";
   // if (isFromEndOfRun) tmpfile = dirName + "/" + taskName + "-" + timestr + "-EOR.root"; 
      msg << MSG::DEBUG << "updating infofile status" << endreq;
    m_infoFileStatus.replace(0, m_infoFileStatus.length(), tmpfile);
   

 //   msg << MSG::INFO << "We will save histograms in file " << m_infoFileStatus << endreq;
    f = new TFile(m_infoFileStatus.c_str(),"create");
  }
  if (f!=0) {
    if(! f->IsZombie()) {
      histogramIdentifier(object, idList, reset, save, level, (TDirectory*) f);   
      if (save) {
        f->Close();
        delete f;f=0;
      }    

    }
    else {
      std::string errorTmpfile = "Error Saving Data => Zombie File..!!!!!"; 
      m_infoFileStatus.replace(0, m_infoFileStatus.length(), errorTmpfile);
      msg << MSG::DEBUG << "error opening file "<< m_infoFileStatus << ". Closing it." << endreq;
      if (f->IsOpen()) {f->Close();}
      delete f;f=0;
    }
    m_dimSvcSaveSetLoc->updateService((char*)m_infoFileStatus.c_str());
  }
  else {
     //f=0 because should also be able to reset without saving
     histogramIdentifier(object, idList, reset, save, level, (TDirectory*) f);     
  }
  
}

void UpdateAndReset::histogramIdentifier(IRegistry* object, std::vector<std::string> &idList, bool reset, bool save, int &level,
                                         TDirectory* rootdir){
  MsgStream msg( msgSvc(), name() );
  std::vector<IRegistry*> leaves;
  std::vector<IRegistry*>::const_iterator  it;
  try {
//     msg << MSG::INFO << "Looking for histos in object " << object->identifier() << ", level  " << level << endreq;
     SmartIF<IDataManagerSvc> dataManagerSvc(m_histogramSvc);
     if (!dataManagerSvc) {
//       msg << MSG::WARNING << "    Unable to go to the transient store. " << endreq;
       return;
     }
  
     StatusCode sc = dataManagerSvc->objectLeaves(object, leaves);
     if (sc.isFailure()) {
//        msg << MSG::WARNING << "    No histograms found on the transient store." << endreq;
        return;
     }   
      
    for ( it=leaves.begin(); it != leaves.end(); it++ ) {
       const std::string& id = (*it)->identifier();
       if (rootdir !=0) rootdir->cd();
//       msg << MSG::DEBUG << "    Object found: " << id << endreq;
    
       DataObject* dataObject;
       sc = m_histogramSvc->retrieveObject(id, dataObject);
       if (sc.isFailure()) {
//         msg << MSG::WARNING << "Could not retrieve object from TES " << endreq;
         continue;
       }

       IHistogram* histogram = dynamic_cast<AIDA::IHistogram*> (dataObject);
       if ( 0 != histogram) {
         if (save) {
           TH1* hRoot = (TH1*) Gaudi::Utils::Aida2ROOT::aida2root(histogram);
           std::vector<std::string> HistoFullName = Misc::splitString(hRoot->GetName(), "/");
	   hRoot->Write( HistoFullName[HistoFullName.size()-1].c_str() );
  //         msg << MSG::INFO << ", saving name=" << hRoot->GetName() << " directory="
  //          << (hRoot->GetDirectory() ? hRoot->GetDirectory()->GetName() : "none") <<endreq;
	  // should we reset on the root level?
	  // if (reset) hRoot->Reset();
         }
        // msg << MSG::DEBUG << "Resetting histogram" << endreq;
         if (reset) {
	   // msg << MSG::INFO << "Resetting histogram" << endreq;
	    histogram->reset();
	    m_countExecutes=0;
	    }
         idList.push_back(id);
         continue;
       }
       IProfile1D* profile = dynamic_cast<AIDA::IProfile1D*> (dataObject);
       if (0 != profile) {
         if (save)  {
           TProfile* hRoot = (TProfile*) Gaudi::Utils::Aida2ROOT::aida2root(profile);
           std::vector<std::string> HistoFullName = Misc::splitString(hRoot->GetName(), "/");
	   hRoot->Write( HistoFullName[HistoFullName.size()-1].c_str() );
	   //should we reset at the root level?
	   //if (reset) hRoot->Reset();
         }
         if (reset){
	// msg << MSG::DEBUG << "Resetting profile" << endreq;
	    profile->reset();
	    }
         idList.push_back(id);
         continue;
       }
    
       // not an histogram: must be a directory: create corresponding TDirectory
       std::vector<std::string> rootDirs = Misc::splitString(id, "/");
       TDirectory* newdir = rootdir;
       if(NULL != newdir) {
         newdir = rootdir->mkdir(rootDirs[rootDirs.size()-1].c_str());
         newdir->cd();
       }
       int newLevel = level + 1;
       if (newLevel >= 10) continue;
       histogramIdentifier(*it, idList, reset, save, newLevel, newdir);
     }
   }
   catch (const std::exception &ex) {
      msg << MSG::WARNING << "histogramidentifier std::exception: " << ex.what() << endreq;
   }
} 

IRegistry* UpdateAndReset::rootObject(){
  MsgStream msg( msgSvc(), name() );
  std::string  path;
  SmartDataPtr<DataObject> smartDataPtr(m_histogramSvc, path);
 // msg << MSG::DEBUG << "root identifier : " << smartDataPtr->registry()->identifier() << endreq;
  return smartDataPtr->registry();
}



