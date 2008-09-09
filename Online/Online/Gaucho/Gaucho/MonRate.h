#ifndef GAUCHO_MONRATE_H
#define GAUCHO_MONRATE_H 1

#include "Gaucho/MonProfile.h"
#include <map>

class MonRate: public MonProfile {

protected:
  std::map<const std::string, std::pair<double*, std::string*>, std::less<std::string> > m_counterMap;
  std::map<const std::string, std::pair<double*, std::string*>, std::less<std::string> >::iterator m_counterMapIt;
  
  int    *m_runNumber;       // Maybe we have to use double
  int    *m_cycleNumber;
  double *m_deltaT;
  double *m_offsetTimeFirstEvInRun;
  double *m_offsetTimeLastEvInCycle; 
  double *m_offsetGpsTimeLastEvInCycle; 
  
  int m_maxNumCounters;
  int m_numCounters;
  bool isServer;
  
    
public:
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  MonRate(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~MonRate();

  virtual void save(boost::archive::binary_oarchive & ar, const unsigned int version);
  virtual void load(boost::archive::binary_iarchive  & ar, const unsigned int version);

  void addCounter(const std::string& countName, const std::string& countDescription, const double& count) {
    
    std::string* descr = new std::string(countDescription);
    m_counterMap[countName] = std::pair<double*, std::string*> (const_cast<double *>(&count), descr);
  }
  
  void addComplement(int* runNumber, int* cycleNumber, double* deltaT, double* offsetTimeFirstEvInRun, double* offsetTimeLastEvInCycle, double* offsetGpsTimeLastEvInCycle){
    m_runNumber = runNumber;
    m_cycleNumber = cycleNumber;
    m_deltaT = deltaT;
    m_offsetTimeFirstEvInRun = offsetTimeFirstEvInRun;
    m_offsetTimeLastEvInCycle = offsetTimeLastEvInCycle;
    m_offsetGpsTimeLastEvInCycle = offsetGpsTimeLastEvInCycle;
  }
  
  std::map<const std::string, std::pair<double*, std::string*>, std::less<std::string> > counterMap(){return m_counterMap;}
  virtual void combine(MonObject * monObject);
  virtual void copyFrom(MonObject* monObject);
  virtual void reset();
  virtual void print();
  virtual void write(){};
  void setMaxNumCounters(int maxNumCounters) {m_maxNumCounters = maxNumCounters;}
  int numCounters(){return m_numCounters;}
  void setNumCounters(int numCounters){m_numCounters = numCounters;}
private:
  double counter(std::string countName) {return (*(m_counterMap[countName].first));}
  std::string counterDescription(std::string countName) {return (*(m_counterMap[countName].second));}
  double offsetTimeFirstEvInRun() {return (*m_offsetTimeFirstEvInRun);}
  double offsetTimeLastEvInCycle() {return (*m_offsetTimeLastEvInCycle);}
  double offsetGpsTimeLastEvInCycle() {return (*m_offsetGpsTimeLastEvInCycle);}
  int runNumber() {return (*m_runNumber);}
  int cycleNumber() {return (*m_cycleNumber);}
  double deltaT() {return (*m_deltaT);}
};

#endif //GAUCHO_MONRATE_H

