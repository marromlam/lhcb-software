// $Id: 
#ifndef   BANKKILLER_H
#define   BANKKILLER_H 1

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"
// from Event
#include "Event/RawEvent.h"

/** @class bankKiller bankKiller.h   
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 Septembre 2006
 */

class bankKiller : public GaudiAlgorithm { 
  friend class AlgFactory<bankKiller>;
 public:
  
  bankKiller( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~bankKiller();
  
  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  
 protected:
  
 private:   
  std::map<std::string,LHCb::RawBank::BankType> m_bankMap;
  std::vector< std::string > m_bankTypes;
};
#endif //    BANKKILLER_H
