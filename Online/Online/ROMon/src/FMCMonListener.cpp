// $Id: FMCMonListener.cpp,v 1.2 2008-06-25 22:53:23 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FMCMonListener.cpp,v 1.2 2008-06-25 22:53:23 frankb Exp $

// C++ include files
#include <cstdlib>
#include "sys/timeb.h"

// Framework includes
#include "dic.hxx"
#include "RTL/strdef.h"
#include "ROMon/ROMonInfo.h"
#include "ROMon/FMCMonListener.h"
#include "ROMonDefs.h"

using namespace ROMon;

/// Standard constructor
FMCMonListener::FMCMonListener(bool verbose) 
  : RODimListener(verbose), m_type(MONITORED)
{
}

/// Standard destructor
FMCMonListener::~FMCMonListener() {
  delete m_dns;
  m_dns = 0;
  dim_lock();
  for(Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    Item* it = (*i).second;
    ::dic_release_service(it->id);
    it->data<Descriptor>()->release();
    it->release();
  }
  dim_unlock();
}

/// Add handler for a given message source
void FMCMonListener::addHandler(const std::string& node,const std::string& svc) {
  dim_lock();
  Clients::iterator i=m_clients.find(node);
  if ( i == m_clients.end() )  {
    for(Match::const_iterator j=m_match.begin(); j!=m_match.end();++j) {
      if ( ::str_match_wild(svc.c_str(), (*j).first.c_str()) )  {
	Item* itm = Item::create<Descriptor>(this);
	std::string nam = svc + "/" + (*j).second;
	m_clients[node] = itm;
	itm->id = ::dic_info_service((char*)nam.c_str(),m_type,0,0,0,infoHandler,(long)itm,0,0);
	if ( m_verbose ) log() << "[FMCMonListener] Create DimInfo:" 
			       << nam << "@" << node << " id=" << itm->id << std::endl;
      }
    }
  }
  dim_unlock();
}

/// Remove handler for a given message source
void FMCMonListener::removeHandler(const std::string& node, const std::string& svc)   {
  dim_lock();
  Clients::iterator i=m_clients.find(node);
  if ( i != m_clients.end() ) {
    Item* it = (*i).second;
    it->data<Descriptor>()->release();
    ::dic_release_service(it->id);
    if ( m_verbose )   {
      log() << "[FMCMonListener] Delete DimInfo:" 
	    << svc << "@" << node << " " << it->id << std::endl;
    }
    it->release();
    m_clients.erase(i);
  }
  dim_unlock();
}

/// DimInfo overload to process messages
void FMCMonListener::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    timeb tm;
    int len = *size;
    Item* it = *(Item**)tag;
    Descriptor* d = it->data<Descriptor>();
    RODimListener* l = (RODimListener*)it->object;
    if ( d->len < len ) {
      d->len = len;
      ::free(d->data);
      d->data = (char*)::malloc(d->len);
    }
    d->actual = len;
    ::ftime(&tm);
    d->time = tm.time;
    d->millitm = tm.millitm;
    ::memcpy(d->data,address,d->actual);
    if ( l && l->updateHandler() ) l->updateHandler()->update();
  }
}
