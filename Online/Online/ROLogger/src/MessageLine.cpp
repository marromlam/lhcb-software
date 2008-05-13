
#include "ROLogger/MessageLine.h"
#include <iostream>


using namespace ROLogger;

MessageLine::MessageLine(const MessageLine& c) 
  : m_buff(c.m_buff), m_type(c.m_type), m_comp(c.m_comp), m_mess(c.m_mess)
{
}

int MessageLine::analyze() const {
  size_t idp, idq, idx = m_buff.find("(",NOD_START);
  if ( idx != std::string::npos && idx<32+NOD_START ) {
    m_utgid = idx+1;
    idq = m_buff.find("):",++idx);
    if ( idq != std::string::npos && idq>idx ) {
      idp = m_buff.find(":",idq+3);
      if ( idp != std::string::npos ) {
	m_mess = idp+2;
	m_comp = idq+3;
	m_type = idx-1;
	return m_type;
      }
    }
  }
  m_mess = m_buff.find(":",NOD_START);
  if ( m_mess != std::string::npos ) m_mess += 2;
  return m_type=OTHER;
}

void MessageLine::dump()  const {
  std::cout << "Line:" << m_buff << std::endl
	    << "  Type:" << type();
  switch(type()) {
  case NONE:    
    std::cout << "[NONE]";
    break;
  case OTHER:
    std::cout << "[OTHER]";
    break;
  default:
    std::cout << "[TASK]";
    break;
  }
  std::cout << " Severity:" << severity()
	    << " Node:" << node();
  if ( type() > 0 ) {
    std::cout << " Utgid:" << utgid() << " Component:" << component();
  }
  std::cout << std::endl << "  Message:" << message() << std::endl;
}

extern "C" int test_MessageLine(int,char**) {
  const char* lines[] = {
    "May08-133647[INFO] mona0803: Gaudi.exe(LHCb_MONA0803_MonEvents_00): StoreExplorerAlg: | | +--> /DAQ [Address: CLID=0x1 Type=0xa000000]  DataObject",
    "May08-133709[DEBUG]mona0804: LogGaudiSrv(/mona0804/LogGaudiSrv): main(): -- MARK --",
    "May08-133808[INFO] mona08: Hello"
  };
  for(size_t i=0; i<sizeof(lines)/sizeof(lines[0]);++i) {
    MessageLine line(lines[i]);
    line.dump();
    std::cout << std::endl;
  }
  return 1;
}
