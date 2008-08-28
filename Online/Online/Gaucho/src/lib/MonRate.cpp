#include "Gaucho/MonRate.h"

MonRate::MonRate(IMessageSvc* msgSvc, const std::string& source, int version):
  MonProfile(msgSvc, source, version)
{
  m_typeName=s_monRate;
  m_dimPrefix="MonR";
  m_runNumber = new int(0);
  m_cycleNumber = new int(0);
  m_deltaT = new double(0);
  m_timeFirstEvInRun = new ulonglong(0);
  m_timeLastEvInCycle = new ulonglong(0);
  m_gpsTimeLastEvInCycle = new ulonglong(0);
  isServer = true;
}
  
MonRate::~MonRate(){
}

void MonRate::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar, version);
  MsgStream msg = createMsgStream();
//   msg <<MSG::INFO<<"**********************************************" << endreq;
  
  if (isServer) {
    //msg <<MSG::INFO<<"THIS IS A SERVER " << endreq;
    int size = m_counterMap.size();
//     msg <<MSG::INFO<<"size: " << size << endreq;
    m_profile = new TProfile("profile","MonRate Profile", size+7, 0, size+7);
    m_profile->Fill(0.50, (double) (*m_timeFirstEvInRun), 1.00);
    m_profile->Fill(1.50, (double) (*m_timeLastEvInCycle), 1.00);
    m_profile->Fill(2.50, (double) (*m_gpsTimeLastEvInCycle), 1.00);
    m_profile->Fill(3.50, 1.00, 1.00);
    m_profile->Fill(4.50, (double) (*m_runNumber), 1.00);
    m_profile->Fill(5.60, (double) (*m_cycleNumber), 1.00);
    m_profile->Fill(6.50, (double) (*m_deltaT), 1.00);
    
/*    msg <<MSG::INFO<<"bin[1]=" << (*m_timeFirstEvInRun) << ", TimeFirstEvInRun" << endreq;    
    msg <<MSG::INFO<<"bin[2]=" << (*m_timeLastEvInCycle) << ", TimeLastEvInCycle" << endreq;
    msg <<MSG::INFO<<"bin[3]=" << (*m_gpsTimeLastEvInCycle) << ", GpsTimeLastEvInCycle" << endreq;
    msg <<MSG::INFO<<"bin[4]=" << 1 << ", one" << endreq;
    msg <<MSG::INFO<<"bin[5]=" << (*m_runNumber) << ", RunNumber" << endreq;
    msg <<MSG::INFO<<"bin[6]=" << (*m_cycleNumber) << ", CycleNumber" << endreq;
    msg <<MSG::INFO<<"bin[7]=" << (*m_deltaT) << ", deltaT" << endreq;*/
    int i = 8;
    for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); m_counterMapIt++) {
      m_profile->Fill((double)i - 0.5, (double)(*(m_counterMapIt->second.first)), 1.00);
//       msg <<MSG::INFO<<"bin [" << i << "]="<< (double)(*(m_counterMapIt->second.first)) << endreq;
      i++;
    }
    m_profile->GetXaxis()->SetBinLabel(1, "TimeFirstEvInRun");
    m_profile->GetXaxis()->SetBinLabel(2, "TimeLastEvInCycle");
    m_profile->GetXaxis()->SetBinLabel(3, "GpsTimeLastEvInCycle");
    m_profile->GetXaxis()->SetBinLabel(4, "One");
    m_profile->GetXaxis()->SetBinLabel(5, "RunNumber");
    m_profile->GetXaxis()->SetBinLabel(6, "CycleNumber");
    m_profile->GetXaxis()->SetBinLabel(7, "deltaT");
        
    i = 8;
    for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); m_counterMapIt++) {
      //msg <<MSG::INFO<<"name: " << m_counterMapIt->first.c_str()<< endreq;
      //m_profile->GetXaxis()->SetBinLabel(i, m_counterMapIt->first.c_str());
      m_profile->GetXaxis()->SetBinLabel(i, (*(m_counterMapIt->second.second)).c_str());
/*      msg <<MSG::INFO<<"description: " << (*(m_counterMapIt->second.second)).c_str() << endreq;
      m_profile->GetYaxis()->SetBinLabel(i, (*(m_counterMapIt->second.second)).c_str());*/
      i++;
    }
  }  
  else{
    //msg <<MSG::INFO<<"THIS IS A CLIENT " << endreq;
    m_profile =  profile();
  }

  //msg <<MSG::INFO<<"**********************************************" << endreq;
  
  //msg <<MSG::INFO<<"size" << endreq;
  //msg <<MSG::INFO<<m_profile->GetNbinsX() << endreq;
/*  for (int j = 0; j< m_profile->GetNbinsX()+2; j++) {
    msg <<MSG::INFO<<"bin["<<j<<"] = " << (ulonglong) m_profile->GetBinContent(j) << ", name: "<< m_profile->GetXaxis()->GetBinLabel(j) << ", entries: " << m_profile->GetBinEntries(j) << endreq;
  }*/
  MonProfile::save(ar, version);
  if (isServer) {
    delete m_profile; m_profile = 0;
  }
}

void MonRate::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MsgStream msg = createMsgStream();
  isServer = false;
  MonObject::load(ar, version);
  MonProfile::load(ar, version);
  
/*  m_profile = profile();
  for (int j = 0; j< m_profile->GetNbinsX()+2; j++) {
    msg <<MSG::INFO<<"bin["<<j<<"] = " << (ulonglong) m_profile->GetBinContent(j) << ", name: "<< m_profile->GetXaxis()->GetBinLabel(j) << ", entries: " << m_profile->GetBinEntries(j) << endreq;
  }*/
  
}

void MonRate::combine(MonObject * monObject) {
  MsgStream msg = createMsgStream();
  if (monObject->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<< monObject->typeName() << " failed." << endreq;
    return;
  }
  if (monObject->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  MonProfile::combine(monObject);  
}

void MonRate::copyFrom(MonObject * monObject){
  isServer = false;
  if (monObject->typeName() != this->typeName()) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  MonProfile::copyFrom(monObject);
}

void MonRate::reset(){
  MonProfile::reset();
}

void MonRate::print(){
  //MonObject::print();
  MonProfile::print();
/*  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << " runNumber: "<<  runNumber() << endreq;
  msgStream << MSG::INFO << " cycleNumber: "<<  cycleNumber() << endreq;
  msgStream << MSG::INFO << " deltaT: "<<  deltaT() << endreq;
  msgStream << MSG::INFO << " timeFirstEvInRun: "<<  timeFirstEvInRun() << endreq;
  msgStream << MSG::INFO << " timeLastEvInCycle: "<<  timeLastEvInCycle() << endreq;
  msgStream << MSG::INFO << " gpsTimeLastEvInCycle: "<<  gpsTimeLastEvInCycle() << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " counter size :"<< m_counterMap.size() << endreq;
  msgStream << MSG::INFO << m_typeName << " values :" << endreq;
  
  for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); ++m_counterMapIt)
    msgStream << MSG::INFO << " counter: "<<  m_counterMapIt->first << ", value: " << (*m_counterMapIt->second.first) << ", description: "<< (*m_counterMapIt->second.second) << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;*/
}

