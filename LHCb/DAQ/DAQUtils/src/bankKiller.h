#ifndef BANKKILLER_H
#define BANKKILLER_H 1

// STL
#include <string>
#include <vector>
#include <map>

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"
#include "DAQKernel/DecoderAlgBase.h"
// from Event
#include "Event/RawEvent.h"

/** @class bankKiller bankKiller.h   
 *
 *   Killer of raw event banks.
 *
 *   Input: 
 *    - BankTypes : list of bank names to be killed or saved
 *    - DefaultIsKill : Main behaviour switch. If false (default), kill only given banks. If true, kill all BUT given banks.
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 Septembre 2006
 */

class bankKiller : public Decoder::AlgBase { 
  friend class AlgFactory<bankKiller>;
 public:
  
  bankKiller( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~bankKiller();
  
  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  
 protected:
  ///. kill all banks of given type
  void  killBankType( LHCb::RawBank::BankType bankType, 
		      bool warningmsg ); 
  
  ///. kill all banks with the current m_rawEvent
  StatusCode  killFromRawEvent( ); 
  
 private:   
  
  std::vector< std::string > m_bankTypes;  ///< List of bank names
  /// Main behaviour switch. If false (default), kill only given banks. If true, kill all BUT given banks.
  bool m_defaultIsKill ;                   
  LHCb::RawEvent* m_rawEvt;
  //kill from all raw event locations, or just from the first one?
  bool m_killFromAll ;   
};
#endif //    BANKKILLER_H
