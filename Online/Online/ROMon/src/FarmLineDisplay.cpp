// $Id: FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $

// Framework include files
#include "ROMon/FarmLineDisplay.h"
#include "SCR/MouseSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <algorithm>

using namespace ROMon;
using namespace SCR;
using namespace std;
typedef vector<string>               StringV;

// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 15
#define CLUSTERLINE_START      2
#define CLUSTERLINE_FIRSTPOS   6

static FarmLineDisplay* s_fd = 0;

static void help() {
  cout << "  romon_farm -option [-option]" << endl
            << "       -all                         Show all subfarms." << endl
            << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
            << "       -an[chor]=+<x-pos>+<ypos>    Set anchor for sub displays" << endl
            << endl;
}


namespace {
  struct DisplayUpdate {
    Pasteboard* m_pb;
    bool m_flush;
    DisplayUpdate(InternalDisplay* d, bool flush=false) : m_pb(d->pasteboard()), m_flush(flush) {
      ::scrc_begin_pasteboard_update(m_pb);
    }
    ~DisplayUpdate() {
      ::scrc_end_pasteboard_update (m_pb);
      if ( m_flush ) ::scrc_fflush(m_pb);
    }
  };
  /// Extract node/service name from DNS info
  void getServiceNode(char* s, string& svc, string& node) {
    char* at = strchr(s,'@');
    *at = 0;
    svc = s;
    node = at+1;
  }
}

namespace ROMon {
  /**@class FarmClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class FarmClusterLine : public ClusterLine  {
    int               m_evtBuilt;
    int               m_evtMoore;
    int               m_evtSent;
    int               m_totBuilt;
    int               m_totMoore;
    int               m_totSent;
    int               m_numUpdate;
    bool              m_hasProblems;
    bool              m_inUse;
    /// Timestamp with last update
    time_t            m_lastUpdate;
  public:
    /// Initializing constructor
    FarmClusterLine(FarmLineDisplay* p, int pos, const std::string& n);
    /// Default destructor
    virtual ~FarmClusterLine() {}
    /// Display function drawing on pasteboard of current display
    void display();
  };

  /**@class MonitoringClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   @author M.Frank
   */
  class MonitoringClusterLine : public ClusterLine  {
    int               m_evtRelay;
    int               m_evtWorker;
    int               m_totRelay;
    int               m_totWorker;
    int               m_numUpdate;
    bool              m_hasProblems;
    bool              m_inUse;
    std::string       m_partition;
    std::string       m_relayNode;
    /// Timestamp with last update
    time_t            m_lastUpdate;
  public:
    /// Initializing constructor
    MonitoringClusterLine(FarmLineDisplay* p, int pos, const std::string& n);
    /// Default destructor
    virtual ~MonitoringClusterLine() {}
    /// Display function drawing on pasteboard of current display
    void display();
  };

  /**@class StorageClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   @author M.Frank
   */
  class StorageClusterLine : public ClusterLine   {
    int               m_evtRecv;
    int               m_evtSent;
    int               m_totRecv;
    int               m_totSent;
    int               m_numUpdate;
    bool              m_hasProblems;
    bool              m_inUse;
    std::string       m_partition;
    std::string       m_relayNode;
    /// Timestamp with last update
    time_t            m_lastUpdate;
  public:
    /// Initializing constructor
    StorageClusterLine(FarmLineDisplay* p, int pos, const std::string& n);
    /// Default destructor
    virtual ~StorageClusterLine() {}
    /// Display function drawing on pasteboard of current display
    void display();
  };
}

#include "ROMon/TaskSupervisor.h"
#include "TaskSupervisorParser.h"

namespace ROMon {

  /**@class CtrlFarmClusterLine ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   @author M.Frank
   */
  class CtrlFarmClusterLine : public ClusterLine, public Interactor   {
    typedef std::set<std::string> StrSet;
    /// Flag to indicate probles with entity
    bool                m_hasProblems;
    /// Service ID for excluded nodes
    int                 m_exclID;
    /// Extracted cluster information for all contained nodes
    Cluster             m_cluster;
    /// Set of excluded nodes
    StrSet              m_excluded;
    /// Timestamp with last update
    time_t              m_lastUpdate;

    /// DIM command service callback
    void updateContent(XML::TaskSupervisorParser& ts);
    /// DIM command service callback
    static void excludedHandler(void* tag, void* address, int* size);

  public:
    /// Initializing constructor
    CtrlFarmClusterLine(FarmLineDisplay* p, int pos, const std::string& n);
    /// Default destructor
    virtual ~CtrlFarmClusterLine();
    /// Display function drawing on pasteboard of current display
    void display();
    /// Interactor overload: Display callback handler
    void handle(const Event& ev);
  };
}

namespace ROMon {
  ClusterLine* createFarmClusterLine(FarmLineDisplay* parent, int pos, const string& title);
  ClusterLine* createRecFarmClusterLine(FarmLineDisplay* parent, int pos, const string& title);
  ClusterLine* createCtrlFarmClusterLine(FarmLineDisplay* parent, int pos, const string& title);
  ClusterLine* createMonitoringClusterLine(FarmLineDisplay* parent, int pos, const string& title);
  ClusterLine* createStorageClusterLine(FarmLineDisplay* parent, int pos, const string& title);

  ClusterLine* createFarmClusterLine(FarmLineDisplay* parent, int pos, const string& title) 
  {    return new FarmClusterLine(parent,pos,title);  }
  ClusterLine* createRecFarmClusterLine(FarmLineDisplay* parent, int pos, const string& title)
  {    return new FarmClusterLine(parent,pos,title);  }
  ClusterLine* createCtrlFarmClusterLine(FarmLineDisplay* parent, int pos, const string& title)
  {    return new CtrlFarmClusterLine(parent,pos,title);  }
  ClusterLine* createMonitoringClusterLine(FarmLineDisplay* parent, int pos, const string& title)
  {    return new MonitoringClusterLine(parent,pos,title);  }
  ClusterLine* createStorageClusterLine(FarmLineDisplay* parent, int pos, const string& title)
  {    return new StorageClusterLine(parent,pos,title);  }
}


#include <limits>
namespace {
  const char* _procNam(const char* nam) {
    const char* p;
    if (0 != ::strstr(nam,"MEPRx") ) return nam;
    p = ::strchr(nam,'_');
    if ( 0 != p ) return ++p;
    return "Unknown";
  }
}

#define COL_WARNING         (RED|BOLD)
#define COL_ALARM           (RED|BOLD|INVERSE)


/// Standard constructor
CtrlFarmClusterLine::CtrlFarmClusterLine(FarmLineDisplay* p, int pos, const std::string& n)
  : ClusterLine(p,pos,n), m_exclID(0)
{
  m_lastUpdate = time(0);
  m_hasProblems = false;
  connect(strupper(m_name)+"/TaskSupervisor/Status");
  string svc   = "HLT/ExcludedNodes/"+strupper(m_name);
  m_exclID = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,excludedHandler,(long)this,0,0);
}

/// Standard destructor
CtrlFarmClusterLine::~CtrlFarmClusterLine() {
  if ( m_exclID ) ::dic_release_service(m_exclID);
  m_exclID = 0;
}

void CtrlFarmClusterLine::display() {
  const char* data = (const char*)cluster();
  try {
    m_lastUpdate = time(0);
    XML::TaskSupervisorParser ts;
    if ( ts.parseBuffer(m_name, data,::strlen(data)+1) ) {
      updateContent(ts);
      return;
    }
  }
  catch(...) {
  }
  size_t         pos = position();
  Pasteboard*     pb = m_parent->pasteboard();
  Display*       dis = m_parent->display();
  RTL::Lock lock(InternalDisplay::screenLock());
  ::scrc_begin_pasteboard_update (pb);
  ::scrc_put_chars(dis," Received invalid XML information",COL_ALARM,pos,1,1);    
  ::scrc_end_pasteboard_update(pb);
}

/// DIM command service callback
void CtrlFarmClusterLine::updateContent(XML::TaskSupervisorParser& ts) {
  char txt[256];
  string val, err;
  bool cl_good = true;
  Cluster& c = m_cluster;
  Cluster::Nodes::const_iterator i, e;
  Pasteboard*     pb = m_parent->pasteboard();
  Display*       dis = m_parent->display();
  int col = NORMAL, pos = 0, line=position();
  size_t taskCount=0, missTaskCount=0;
  size_t connCount=0, missConnCount=0;
  int pvss_status=0;
  c.nodes.clear();
  ts.getClusterNodes(c);

  RTL::Lock lock(InternalDisplay::screenLock());
  ::scrc_begin_pasteboard_update (pb);

  ::scrc_put_chars(dis,"", NORMAL,line,11,1);
  pos = 87+CLUSTERLINE_START;
  for(i=c.nodes.begin(), e=c.nodes.end(); i!=e;++i) {
    const Cluster::Node& n = (*i).second;
    bool excl = m_excluded.find(n.name) != m_excluded.end();
    bool good = (n.status == "ALIVE");
    for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q) {
      bool pvss_ok = (*q).eventMgr && (*q).dataMgr && (*q).distMgr;
      if ( pvss_ok && pvss_status<2 ) pvss_status=1;
      else if ( !pvss_ok ) pvss_status = 2;
      good = good && pvss_ok;
    }
    col = good && n.missTaskCount==0 && n.missConnCount==0 ? GREEN|INVERSE : COL_ALARM;
    if ( excl )  {
      col = INVERSE|(col==COL_ALARM ? MAGENTA : BLUE);
    }
    else {
      taskCount     += n.taskCount;
      missTaskCount += n.missTaskCount;
      connCount     += n.connCount;
      missConnCount += n.missConnCount;
    }
    val = (n.name == m_name ? " "+n.name : " "+n.name.substr(n.name.length()-2));
    if ( ::strncasecmp(n.name.c_str(),"store",4)==0 ) val = " " + n.name;
    ::scrc_put_chars(dis,val.c_str(),col,line,pos,0);
    pos += val.length();
    cl_good |= (good || excl);
  }

  ::scrc_put_chars(dis,"",NORMAL,line,pos,1);
  col = (c.status=="ALIVE" || cl_good) ? NORMAL : (c.status=="MIXED") ? COL_WARNING : COL_ALARM;
  ::scrc_put_chars(dis,strlower(c.name).c_str(),(this==m_parent->currentDisplay()?BLUE|INVERSE:NORMAL)|BOLD,line,2,0);
  ::sprintf(txt,"%8s %6zu ",c.time.c_str()+11,c.nodes.size());
  ::scrc_put_chars(dis,txt,NORMAL,line,12,0);

  if ( missTaskCount>0 ) {
    ::sprintf(txt,"%4zu/BAD ",missTaskCount);
    ::scrc_put_chars(dis,txt,COL_ALARM,line,28,0);
    ::scrc_put_chars(dis," ",NORMAL,line,37,0);
  }
  else {
    ::sprintf(txt,"%4zu ",taskCount);
    ::scrc_put_chars(dis,txt,GREEN|INVERSE,line,28,0);
    ::scrc_put_chars(dis,"     ",NORMAL,line,33,0);
  }
  if ( missConnCount>0 ) {
    ::sprintf(txt,"%4zu/BAD ",missConnCount);
    ::scrc_put_chars(dis,txt,COL_ALARM,line,37,0);  
    ::scrc_put_chars(dis," ",NORMAL,line,46,0);
  }
  else {
    ::sprintf(txt,"%4zu ",connCount);
    ::scrc_put_chars(dis,txt,GREEN|INVERSE,line,37,0);  
    ::scrc_put_chars(dis,"     ",NORMAL,line,42,0);
  }
  ::sprintf(txt,"  %-6s  ",c.status.c_str());
  ::scrc_put_chars(dis,txt,col|BOLD,line,45,0);
  //bg_black(pb);
  ::scrc_put_chars(dis,pvss_status>1?"ERROR":"   OK",pvss_status>1?COL_ALARM:NORMAL|BOLD|GREEN,line,53,0);    

  col = NORMAL|BOLD;

  if ( pvss_status>1 )
    err = " PVSS environment looks funny.", col = COL_ALARM;    
  else if ( !cl_good && c.status == "DEAD" )
    err = " Nodes down - Check.", col = COL_WARNING;    
  else if ( c.status == "MIXED" )
    err = " Some nodes down - Check.", col = COL_WARNING;
  else if ( missTaskCount>0 )
    err = " Tasks missing - Check.", col = COL_WARNING;
  else if ( missConnCount>0 )
    err = " Connectivity bad - Check.", col = COL_WARNING;
  else
    err = " No obvious error detected.", col = NORMAL|BOLD|GREEN;
  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,30).c_str(),col,line,57+CLUSTERLINE_START,0);

  end_update();
}

/// Interactor overload: Display callback handler
void CtrlFarmClusterLine::handle(const Event& ev) {
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type) {
    case CMD_EXCLUDE:
      m_excluded = *auto_ptr<StrSet>(ev.iocPtr<StrSet>()).get();
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// DIM command service callback
void CtrlFarmClusterLine::excludedHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    CtrlFarmClusterLine* l = *(CtrlFarmClusterLine**)tag;
    char *p = (char*)address, *end = p+*size;
    set<string> nodes;
    while(p<end) {
      nodes.insert(strlower(p));
      p += (::strlen(p)+1);
    }
    IocSensor::instance().send(l,CMD_EXCLUDE,new set<string>(nodes));
  }
}

/// Standard constructor
StorageClusterLine::StorageClusterLine(FarmLineDisplay* p, int pos, const std::string& n)
: ClusterLine(p,pos,n)
{
  m_lastUpdate = time(0);
  m_hasProblems = false;
  m_partition = p->name();
  connect(strlower(m_name)+"/ROpublish");
}

void StorageClusterLine::display() {
  typedef MBMBuffer::Clients           Clients;
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
  char text[256], txt[256];
  size_t         pos = position();
  Pasteboard*     pb = m_parent->pasteboard();
  Display*       dis = m_parent->display();
  const Nodeset*   c = cluster();
  const Nodes& nodes = c->nodes;
  string err = "";
  int col = NORMAL;
  bool inuse = false;
  set<string> bad_nodes;
  static const int   INT_max = numeric_limits<int>::max();
  static const float FLT_max = numeric_limits<float>::max();

  string evt_buff = std::string("Events_"+m_partition);
  int numNodes = 0, numBuffs = 0, numClients = 0;
  float fsp, fsl, fspace[3] = {FLT_max,FLT_max,FLT_max}, fslots[3] = {FLT_max,FLT_max,FLT_max};
  int tot_prod[3] = {0,0,0}, min_prod[3] = {INT_max,INT_max,INT_max};
  int   num_cl[3] = {0,0,0}, num_sl[3] = {0,0,0};

  RTL::Lock lock(InternalDisplay::screenLock());
  ::scrc_begin_pasteboard_update (pb);

  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    bool recv_node = ::strncasecmp((*n).name,"storerecv",8) == 0;
    const Buffers& buffs = *(*n).buffers();
    ++numNodes;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const char* bnam = (*ib).name;
      if ( ::strstr(bnam,m_partition.c_str()) != 0 ) {
        int idx = (recv_node && evt_buff == bnam) ? 0 : (evt_buff == bnam) ? 1 : -1;
        if ( idx >= 0 )   {
          const MBMBuffer::Control& ctrl = (*ib).ctrl;
          ++numBuffs;
          tot_prod[idx] += ctrl.tot_produced;
          numClients    += (*ib).clients.size();
          num_cl[idx]   += (*ib).clients.size();
          num_sl[idx]   += ctrl.p_emax - ctrl.i_events;
          min_prod[idx]  = min(min_prod[idx],ctrl.tot_produced);
	  fsp = float(ctrl.i_space)/float(ctrl.bm_size);
	  fsl = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
          fspace[idx]    = min(fspace[idx],fsp);
          fslots[idx]    = min(fslots[idx],fsl);
          if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) bad_nodes.insert((*n).name);
          inuse = true;
        }
      }
    }
  }
  char b1[64];
  TimeStamp frst=c->firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, now = time(0);
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %8s %6d %6d %6d   ",b1,numNodes,numBuffs,numClients);
  ::scrc_put_chars(dis," ",NORMAL,pos,1,1);
  ::scrc_put_chars(dis,c->name,(this==m_parent->currentDisplay()?BLUE|INVERSE:NORMAL)|BOLD,pos,2,0);
  ::scrc_put_chars(dis,text,NORMAL,pos,12,0);
  m_inUse = inuse;

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }

  if ( tot_prod[0] != 0 )
    ::sprintf(txt,"Evt:%9d Cl:%4d Sl:%5d%17s",tot_prod[0],num_cl[0],num_sl[0],"");
  else
    ::sprintf(txt,"%13s%8s%9s%64s","--","--","--","");
  ::scrc_put_chars(dis," Receive:",BOLD,pos,77+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+9+CLUSTERLINE_START,0);

  if ( tot_prod[1] != 0 )
    ::sprintf(txt,"Evt:%9d Cl:%4d Sl:%5d%17s",tot_prod[1],num_cl[1],num_sl[1],"");
  else
    ::sprintf(txt,"%13s%8s%9s%64s","--","--","--","");
  ::scrc_put_chars(dis,"  Stream:",BOLD,pos,77+37+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+46+CLUSTERLINE_START,0);

  m_hasProblems = true;
  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    err = " No update information available", col = RED|INVERSE|BOLD;
  }
  else if ( numNodes == 0 ) {
    err = " No nodes found!", col = BOLD|RED|INVERSE;
  }
  else if ( !inuse ) {
    err = " Storage not used yet....", col = NORMAL|INVERSE|GREEN;
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN ) {
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"Recv ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"Stream ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN ) {
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"Recv ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"Stream ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( min_prod[0] != INT_max && min_prod[0]>0 && min_prod[0] <= m_evtRecv ) {
    err = " Receiving layer idle.", col = RED|BOLD;
  }
  else if ( min_prod[1] != INT_max && min_prod[1]>0 && min_prod[1] <= m_evtSent ) {
    err = " Streaming layer idle.", col = RED|BOLD;
  }
  else {
    err = " No obvious Errors detected....", col = NORMAL|INVERSE|GREEN;
  }

  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,35).c_str(),col,pos,42+CLUSTERLINE_START,0);
  m_evtRecv = min_prod[0];
  m_evtSent = min_prod[1];
  m_totRecv = tot_prod[0];
  m_totSent = tot_prod[1];
  end_update();
}


/// Standard constructor
MonitoringClusterLine::MonitoringClusterLine(FarmLineDisplay* p, int pos, const std::string& n)
: ClusterLine(p,pos,n)
{
  m_numUpdate = 0;
  m_evtRelay  = m_totRelay = 0;
  m_evtWorker = m_totWorker = 0;
  m_partition = p->name();
  m_relayNode = m_name+"01";
  m_lastUpdate = time(0);
  m_hasProblems = false;
  connect(strlower(m_name)+"/ROpublish");
}


void MonitoringClusterLine::display() {
  typedef MBMBuffer::Clients           Clients;
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
  char text[256], txt[256];
  size_t         pos = position();
  Pasteboard*     pb = m_parent->pasteboard();
  Display*       dis = m_parent->display();
  const Nodeset*   c = cluster();
  const Nodes& nodes = c->nodes;
  string err = "";
  bool inuse = false;
  set<string> bad_nodes;
  static const int   INT_max = numeric_limits<int>::max();
  static const float FLT_max = numeric_limits<float>::max();

  string evt_buff = "Events_"+m_partition;
  string out_buff = std::string("Output_"+m_partition);
  int numNodes = 0, numBuffs = 0, numClients = 0;
  int tot_prod[3] = {0,0,0}, num_cl[3] = {0,0,0}, num_sl[3] = {0,0,0};
  int min_prod[3] = {INT_max,INT_max,INT_max};

  float fsp, fspace[3] = {FLT_max,FLT_max,FLT_max};
  float fsl, fslots[3] = {FLT_max,FLT_max,FLT_max};

  RTL::Lock lock(InternalDisplay::screenLock());
  ::scrc_begin_pasteboard_update (pb);

  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    bool relay = ::strncasecmp((*n).name,m_relayNode.c_str(),m_relayNode.length()) == 0;
    const Buffers& buffs = *(*n).buffers();
    ++numNodes;
    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const char* bnam = (*ib).name;
      if ( ::strstr(bnam,m_partition.c_str()) != 0 ) {
        int idx = -1;
        if ( relay && evt_buff == bnam )
          idx = 0;
        else if ( !relay && evt_buff == bnam )
          idx = 1;
        else if ( !relay && out_buff == bnam )
          idx = 2;
        if ( idx >= 0 ) {
          const MBMBuffer::Control& ctrl = (*ib).ctrl;
          int ncl = (*ib).clients.size();
          numClients += ncl;
          ++numBuffs;
	  tot_prod[idx] += ctrl.tot_produced;
	  num_cl[idx]   += ncl;
	  num_sl[idx]   += ctrl.p_emax - ctrl.i_events;
          min_prod[idx]  = min(min_prod[idx],ctrl.tot_produced);
	  fsl            = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
	  fsp            = float(ctrl.i_space)/float(ctrl.bm_size);
          fspace[idx]    = min(fspace[idx],fsp);
          fslots[idx]    = min(fslots[idx],fsl);
          if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) {
            bad_nodes.insert((*n).name);
          }
          inuse = true;
        }
      }
    }
  }
  char b1[64];
  TimeStamp frst=c->firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, now = time(0);
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %8s %6d %6d %6d   ",b1,numNodes,numBuffs,numClients);
  ::scrc_put_chars(dis," ",NORMAL,pos,1,1);
  ::scrc_put_chars(dis,c->name,(this==m_parent->currentDisplay()?BLUE|INVERSE:NORMAL)|BOLD,pos,2,0);
  ::scrc_put_chars(dis,text,NORMAL,pos,12,0);
  m_inUse = inuse;

  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;

  if ( tot_prod[0] != 0 )
    ::sprintf(txt,"Evt:%9d Cl:%4d Sl:%5d%64s",tot_prod[0],num_cl[0],num_sl[0],"");
  else
    ::sprintf(txt,"%13s%8s%9s%64s","--","--","--","");
  ::scrc_put_chars(dis,"   Relay:",BOLD,pos,77+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+9+CLUSTERLINE_START,0);

  if ( tot_prod[1] != 0 && tot_prod[2] != 0 )
    ::sprintf(txt,"Evt:%9d Cl:%4d Sl:%5d Evt:%9d Cl:%4d Sl:%4d%64s",
	      tot_prod[1],num_cl[1],num_sl[1],tot_prod[2],num_cl[2],num_sl[2],"");
  else if ( tot_prod[1] != 0 )
    ::sprintf(txt,"Evt:%9d Cl:%4d Sl:%5d  %-62s",tot_prod[1],num_cl[1],num_sl[1],"No Output streams");
  else
    ::sprintf(txt,"%13s%8s%9s%64s","--","--","--","");
  ::scrc_put_chars(dis,"  Worker:",BOLD,pos,77+37+CLUSTERLINE_START,0);
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+46+CLUSTERLINE_START,0);
  
  int col = NORMAL;
  if ( now-m_lastUpdate > UPDATE_TIME_MAX ) {
    err = " No update information available", col = RED|INVERSE|BOLD;
  }
  else if ( numNodes == 0 ) {
    err = " No nodes found in this cluster!", col = BOLD|RED|INVERSE;
  }
  else if ( !inuse ) {
    err = " Monitoring cluster not used yet....", col = NORMAL|INVERSE|GREEN;
  }
  else if ( fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || fslots[2] < SLOTS_MIN ) {
    ::sprintf(txt," SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"Relay ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"Events ");
    if ( fslots[2] < SLOTS_MIN ) ::strcat(txt,"Output ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || fspace[2] < SPACE_MIN  ) {
    ::sprintf(txt," SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"Relay ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"Events ");
    if ( fspace[2] < SPACE_MIN ) ::strcat(txt,"Output ");
    ::sprintf(text,"[%d nodes]",int(bad_nodes.size()));
    ::strcat(txt,text);
    err = txt, col = BOLD|RED|INVERSE;
  }
  else if ( tot_prod[0]>0 && tot_prod[0] <= m_totRelay ) {
    err = " No RELAY activity visible.", col = BOLD|RED;
  }
  else if ( tot_prod[1]>0 && tot_prod[1] <= m_totWorker ) {
    err = " No Worker activity visible.", col = BOLD|RED;
  }
  else if ( min_prod[0] != INT_max && min_prod[0]>0 && min_prod[0] <= m_evtRelay ) {
    err = " Relay node looks idle.", col = BOLD|RED;
  }
  else if ( min_prod[1] != INT_max && min_prod[1]>0 && min_prod[1] <= m_evtWorker ) {
    err = " Some workers idle around.", col = BOLD|RED;
  }
  else  {
    err = " No obvious Errors detected....", col = NORMAL|INVERSE|GREEN;
  }
  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,35).c_str(),col,pos,42+CLUSTERLINE_START,0);

  m_evtRelay  = min_prod[0];
  m_totRelay  = tot_prod[0];
  m_evtWorker = min_prod[1];
  m_totWorker = tot_prod[1];

  end_update();
}


FarmClusterLine::FarmClusterLine(FarmLineDisplay* p, int pos, const std::string& n)
: ClusterLine(p,pos,n)
{
  m_numUpdate = 0;
  m_evtSent  = m_totSent = 0;
  m_evtMoore = m_totMoore = 0;
  m_evtBuilt = m_totBuilt = 0;
  m_lastUpdate = time(0);
  m_hasProblems = false;
  connect(strlower(m_name)+"/ROpublish");
}

void FarmClusterLine::display() {
  typedef MBMBuffer::Clients           Clients;
  typedef Node::Buffers                Buffers;
  typedef Nodeset::Nodes               Nodes;
  char ti[128], text[256], txt[256];
  size_t pos = position();
  Pasteboard* pb = m_parent->pasteboard();
  Display*    dis = m_parent->display();
  const Nodeset* c = cluster();
  const Nodes& nodes = c->nodes;
  static const int   INT_max = numeric_limits<int>::max();
  static const float FLT_max = numeric_limits<float>::max();

  int evt_prod[4]    = {0,0,0,0}, min_prod[4]  = {INT_max,INT_max,INT_max,INT_max};
  int free_space[4]  = {0,0,0,0}, min_space[4] = {INT_max,INT_max,INT_max,INT_max};
  int free_slots[4]  = {0,0,0,0}, min_slots[4] = {INT_max,INT_max,INT_max,INT_max};
  int buf_clients[4] = {0,0,0,0};
  float fspace[4]    = {FLT_max,FLT_max,FLT_max,FLT_max};
  float fslots[4]    = {FLT_max,FLT_max,FLT_max,FLT_max};
  float fsl, fsp;
  int evt_sent       = INT_max;
  int evt_moore      = INT_max;
  int evt_built      = INT_max;
  bool inuse         = false;
  int numNodes       = 0;
  int numBuffs       = 0;
  int numClients     = 0;
  set<string> bad_nodes;

  RTL::Lock lock(InternalDisplay::screenLock());
  int col = NORMAL;

  ::scrc_begin_pasteboard_update (pb);
  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    numNodes++;
    int node_evt_mep = 0;
    int node_evt_sent = INT_max;
    int node_evt_moore = INT_max;

    const char* nn = (*n).name;
    txt[1] = nn[0];
    txt[2] = nn[1];
    col = NORMAL;
    ::sprintf(txt," %s ",nn+((::strncmp(nn,c->name,::strlen(c->name)+2) == 0) ? 0 : ::strlen(nn)-2));

    for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      int idx = 0;
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      ++numBuffs;
      switch(b) {
      case MEP_BUFFER:        idx = 0; break;
      case EVT_BUFFER:        idx = 1; break;
      case RES_BUFFER:        idx = 2; break;
      case SND_BUFFER:        idx = 3; break;
      default:                continue;
      }
      inuse = true;
      fsp               = float(ctrl.i_space)/float(ctrl.bm_size);
      fsl               = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
      fspace[idx]       = min(fspace[idx],fsp); 
      fslots[idx]       = min(fslots[idx],fsl);
      min_space[idx]    = min(min_space[idx],(ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024);
      min_slots[idx]    = min(min_slots[idx],ctrl.p_emax-ctrl.i_events);
      min_prod[idx]     = min(min_prod[idx],ctrl.tot_produced);
      evt_prod[idx]    += ctrl.tot_produced;
      free_space[idx]  += (ctrl.i_space*ctrl.bytes_p_Bit)/1024/1024;
      free_slots[idx]  += (ctrl.p_emax-ctrl.i_events);
      buf_clients[idx] += ctrl.i_users;
      if ( fsl < SLOTS_MIN || fsp < SPACE_MIN ) {
        bad_nodes.insert((*n).name);
      }
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        ++numClients;
        const char* p = _procNam((*ic).name);
        switch(*p) {
        case BUILDER_TASK:
          if( b == MEP_BUFFER ) {
            node_evt_mep += (*ic).events;
          }
          break;
        case SENDER_TASK:
          if( b == RES_BUFFER || b == SND_BUFFER )  {
            node_evt_sent = min(node_evt_sent,(*ic).events);
          }
          break;
        case MOORE_TASK:
          //  Normal  and        TAE event processing
          if( b == EVT_BUFFER || b == MEP_BUFFER )  {
            node_evt_moore = min(node_evt_moore,(*ic).events);
          }
          break;
        default:
          break;
        }
      }
    }
    evt_moore = min(evt_moore,node_evt_moore);
    evt_built = min(evt_built,node_evt_mep);
    evt_sent  = min(evt_sent,node_evt_sent);
  }
  TimeStamp frst=c->firstUpdate();
  time_t t1 = numNodes == 0 ? time(0) : frst.first, prev_update=m_lastUpdate;;
  ::strftime(ti,sizeof(ti),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %8s %6d %6d %6d   ",ti,numNodes,numBuffs,numClients);
  ::scrc_put_chars(dis," ",NORMAL,pos,1,1);
  ::scrc_put_chars(dis,c->name,(this==m_parent->currentDisplay()?BLUE|INVERSE:NORMAL)|BOLD,pos,2,0);
  ::scrc_put_chars(dis,text,NORMAL,pos,12,0);

  m_inUse = inuse;
  if ( numNodes != 0 ) {
    m_lastUpdate = t1;
  }
  m_hasProblems = true;
  // If Result buffer is not in use
  if ( buf_clients[3] != 0 && buf_clients[2] == 0 )  {
    buf_clients[2] = buf_clients[3];
    fspace[2]      = fspace[3];
    fslots[2]      = fslots[3];
    min_space[2]   = min_space[3];
    min_slots[2]   = min_slots[3];
    min_prod[2]    = min_prod[3];
    evt_prod[2]    = evt_prod[3];
    free_space[2]  = free_space[3];
    free_slots[2]  = free_slots[3];
  }

  bool slots_min = fslots[0] < SLOTS_MIN || fslots[1] < SLOTS_MIN || fslots[2] < SLOTS_MIN;
  bool space_min = fspace[0] < SPACE_MIN || fspace[1] < SPACE_MIN || fspace[2] < SPACE_MIN;

  col = NORMAL;
  string err = "";
  if ( prev_update-m_lastUpdate > UPDATE_TIME_MAX ) {
    err = " No update information available", col = RED|INVERSE|BOLD;
  }
  else if ( numNodes == 0 ) {
    err = " No nodes found in this subfarm!", col =  RED|INVERSE|BOLD;
  }
  else if ( !inuse ) {
    err = " Subfarm not used by any partition....", col = NORMAL|INVERSE|GREEN;
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0]<m_totBuilt ) {
    err = " Some MEPRx(s) stuck.", col = BOLD|RED|INVERSE;
  }
  else if ( evt_built <= m_evtBuilt && evt_prod[0] == m_totBuilt ) {
    err = " No DAQ activity visible.", col = BOLD|RED;
  }
  else if ( evt_moore+2 <= m_evtMoore && evt_prod[1] > m_totMoore ) {
    err = " Some MOORE(s) stuck.", col = BOLD|RED|INVERSE;
  }
  else if ( evt_moore <= m_evtMoore && evt_prod[1] == m_totMoore ) {
    err = " No HLT activity visible.", col = BOLD|RED;
  }
  else if ( evt_sent+2 <= m_evtSent && evt_prod[2] > m_totSent ) {
    err = " Some Sender(s) stuck.", col = BOLD|RED|INVERSE;
  }
  else if ( evt_sent <= m_evtSent && evt_prod[0] == m_totSent ) {
    err = " No STORAGE activity visible.", col = BOLD|RED;
  }
  else if ( slots_min  ) {
    int nbad = int(bad_nodes.size());
    ::sprintf(txt,"SLOTS at limit:");
    if ( fslots[0] < SLOTS_MIN ) ::strcat(txt,"MEP ");
    if ( fslots[1] < SLOTS_MIN ) ::strcat(txt,"EVENT ");
    if ( fslots[2] < SLOTS_MIN ) ::strcat(txt,"RES/SEND ");
    ::sprintf(text,"[%d nodes]",nbad);
    ::strcat(txt,text);
    // We have 11 slow nodes in a farm: if these are full, this is no error
    err = txt, col = INVERSE|(nbad>0 ? GREEN : RED);
  }
  else if ( space_min  ) {
    int nbad = int(bad_nodes.size());
    ::sprintf(txt,"SPACE at limit:");
    if ( fspace[0] < SPACE_MIN ) ::strcat(txt,"MEP ");
    if ( fspace[1] < SPACE_MIN ) ::strcat(txt,"EVENT ");
    if ( fspace[2] < SPACE_MIN ) ::strcat(txt,"RES/SEND ");
    ::sprintf(text,"[%d nodes]",nbad);
    ::strcat(txt,text);
    // We have 11 slow nodes in a farm: if these are full, this is no error
    err = txt, col = INVERSE|(nbad>0 ? GREEN : RED);
  }
  else  {
    err = " No obvious Errors detected....", col=NORMAL|INVERSE|GREEN;
    m_hasProblems = false;
  }
  m_evtBuilt  = evt_built;
  m_evtMoore  = evt_moore;
  m_evtSent   = evt_sent;
  m_totBuilt  = evt_prod[0];
  m_totMoore  = evt_prod[1];
  m_totSent   = evt_prod[2];

  err = err + "                                                                 ";
  ::scrc_put_chars(dis,err.substr(0,35).c_str(),col,pos,42+CLUSTERLINE_START,0);
  if ( evt_prod[0] != 0 )
    ::sprintf(txt,"%9d%5d%11d%6d%9d%5d",
              evt_prod[0],free_slots[0],
              evt_prod[1],free_slots[1],
              evt_prod[2],free_slots[2]);
  else
    ::sprintf(txt,"%9s%5s%10s%7s%9s%5s","--","--","--","--","--","--");
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+CLUSTERLINE_START,0);
  if ( min_prod[0] != INT_max )
    ::sprintf(txt,"%9d%5d%11d%6d%9d%5d",
              min_prod[0],min_slots[0],
              min_prod[1],min_slots[1],
              min_prod[2],min_slots[2]);
  else
    ::sprintf(txt,"%9s%5s%10s%7s%9s%5s","--","--","--","--","--","--");
  ::scrc_put_chars(dis,txt,NORMAL,pos,77+47+CLUSTERLINE_START,0);
  end_update();
}

/// Standard constructor
ClusterLine::ClusterLine(FarmLineDisplay* p, int pos, const std::string& n)
  : m_name(n), m_svc(0), m_position(pos), m_parent(p), m_ptr(0)
{
  //string svc = InternalDisplay::svcPrefix()+strlower(n)+"/ROpublish";
  //m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

ClusterLine::~ClusterLine() {
  if ( m_svc ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
}

void ClusterLine::check(time_t /* now */) {
}

/// Connect to data service
void ClusterLine::connect(const std::string& nam) {
  string svc = InternalDisplay::svcPrefix()+nam;
  if ( m_svc ) ::dic_release_service(m_svc);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

void ClusterLine::display() {
  char ti[128], text[256];
  size_t pos = position();
  Pasteboard* pb = m_parent->pasteboard();
  Display*    dis = m_parent->display();
  const Nodeset* c = cluster();
  const Nodeset::Nodes& nodes = c->nodes;
  Nodeset::Nodes::const_iterator ci=nodes.begin();

  TimeStamp frst=c->firstUpdate();
  int numNodes=0,numBuffs=0,numClients=0;

  RTL::Lock lock(InternalDisplay::screenLock());

  ::scrc_begin_pasteboard_update (pb);
  ::scrc_put_chars(dis,c->name,(this==m_parent->currentDisplay()?BLUE|INVERSE:NORMAL)|BOLD,pos,2,0);
  time_t t1 = numNodes == 0 ? time(0) : frst.first;

  ::strftime(ti,sizeof(ti),"%H:%M:%S",::localtime(&t1));
  ::sprintf(text," %8s %6d %6d %6d   ",ti,numNodes,numBuffs,numClients);
  ::scrc_put_chars(dis,text,NORMAL,pos,12,0);
  static int upda = 0;
  ++upda;
  int xp = 42+CLUSTERLINE_START;
  for(; ci != nodes.end(); ci = nodes.next(ci) ) {
    char txt[64];
    int col = RED; //(upda%4==0) ? RED : (upda%3==0) ? BLUE : (upda%2==0) ? GREEN : BOLD;
    col |= INVERSE;
    const char* n = (*ci).name;
    txt[1] = n[0];
    txt[2] = n[1];
    ::sprintf(txt," %s ",n+((::strncmp(n,c->name,::strlen(c->name)+2) == 0) ? 0 : ::strlen(n)-2));
    ::scrc_put_chars(dis,txt,col,pos,xp,0);
    xp += ::strlen(txt);
  }

  ::scrc_put_chars(dis," ",NORMAL,pos,xp,1);
  end_update();
}

void ClusterLine::end_update() {
  ClusterDisplay* sfdis = m_parent->subfarmDisplay();
  Pasteboard* pb = m_parent->pasteboard();
  const Nodeset* c = cluster();
  if ( sfdis && sfdis->clusterName() == c->name )  {
    IocSensor::instance().send(m_parent,CMD_CHECK,this);
    //sfdis->updateDisplay(*c);
  }
  ::scrc_end_pasteboard_update(pb);
  if ( sfdis ) {
    m_parent->set_cursor();
    ::scrc_cursor_on(pb);
  }
  else {
    ::scrc_cursor_off(pb);
  }
}

/// DIM command service callback
void ClusterLine::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    ClusterLine* l = *(ClusterLine**)tag;
    char* ptr = new char[*size+sizeof(int)];
    *(int*)ptr = *size;
    ::memcpy(ptr+sizeof(int),address,*size);
    if ( l->m_ptr ) delete [] l->m_ptr;
    l->m_ptr = ptr;
    l->m_cluster = (Nodeset*)((char*)l->m_ptr + sizeof(int));
    l->display();
  }
}

/// Standard constructor
FarmLineDisplay::FarmLineDisplay(int argc, char** argv)
  : FarmDisplayBase(), m_currentLine(0)
{
  char txt[128];
  string anchor, prefix;
  RTL::CLI cli(argc,argv,help);
  bool all = 0 != cli.getopt("all",2);
  bool xml = 0 != cli.getopt("xml",2);
  cli.getopt("partition",   2, m_name = "ALL");
  cli.getopt("match",       2, m_match = "*");
  cli.getopt("prefix",      2, prefix);
  cli.getopt("sdh",         2, m_subDisplayHeight);
  cli.getopt("node-height", 7, m_subDisplayHeight);

  m_dense = 0 != cli.getopt("dense",2);
  m_mode  = cli.getopt("reconstruction",2) == 0 ? HLT_MODE : RECO_MODE;
  if ( cli.getopt("taskmonitor",2) != 0 ) m_mode = CTRL_MODE;
  if ( cli.getopt("anchor",2,anchor) != 0 ) {
    int x, y;
    if ( 2 == ::sscanf(anchor.c_str(),"+%d+%d",&x,&y) ) {
      m_anchorX = x;
      m_anchorY = y;
    }
    else if ( 2 == ::sscanf(anchor.c_str(),"%dx%d",&x,&y) ) {
      m_anchorX = x;
      m_anchorY = y;
    }
    else {
      ::printf("No valid anchor position given.\n");
    }
  }
  if ( !prefix.empty() ) InternalDisplay::setSvcPrefix(prefix);
  s_fd = this;
  if ( m_mode == RECO_MODE && all && m_match=="*" )
    ::sprintf(txt," Reconstruction farm display of all known subfarms ");
  else if ( m_mode == RECO_MODE && all )
    ::sprintf(txt," Reconstruction farm display of all known subfarms with name '%s'",m_match.c_str());
  else if ( m_mode == RECO_MODE )
    ::sprintf(txt," Reconstruction farm display of partition %s ",m_name.c_str());
  else if ( m_mode == CTRL_MODE && all && m_match=="*" )
    ::sprintf(txt," Task Control farm display of all known subfarms ");
  else if ( m_mode == CTRL_MODE && all )
    ::sprintf(txt," Task Control farm display of all known subfarms with name '%s'",m_match.c_str());
  else if ( m_mode == CTRL_MODE )
    ::sprintf(txt," Task Control farm display of partition %s ",m_name.c_str());
  else if ( m_match == "*" && all )
    ::sprintf(txt," HLT Farm display of all known subfarms ");
  else if ( all )
    ::sprintf(txt," HLT Farm display of all known subfarms with the name '%s'",m_match.c_str());
  else
    ::sprintf(txt," HLT Farm display of partition %s ",m_name.c_str());
  m_title = txt;
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  ScrDisplay::setPasteboard(m_pasteboard);
  ScrDisplay::setBorder(BLUE|INVERSE);
  m_width -= 2;
  m_height -= 2;
  ::scrc_create_display (&m_display, m_height, m_width,NORMAL, ON, m_title.c_str());
  show(2,2);
  if ( m_mode == CTRL_MODE ) {
    ::scrc_put_chars(m_display,txt,NORMAL|BOLD,1,2,0);
    ::scrc_put_chars(m_display,"<CTRL-H for Help>, <CTRL-E to exit>",NORMAL|BOLD,2,40,0);
    ::scrc_put_chars(m_display,"nn",GREEN|INVERSE,1,80,0);
    ::scrc_put_chars(m_display,": OK",NORMAL,1,82,0);
    ::scrc_put_chars(m_display,"nn",RED|INVERSE,1,90,0);
    ::scrc_put_chars(m_display,": Not OK",NORMAL,1,92,0);
    ::scrc_put_chars(m_display,"nn",BLUE|INVERSE,1,110,0);
    ::scrc_put_chars(m_display,": OK/Excluded",NORMAL,1,112,0);
    ::scrc_put_chars(m_display,"nn",MAGENTA|INVERSE,1,130,0);
    ::scrc_put_chars(m_display,": Not OK/Excluded",NORMAL,1,132,1);
  }
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
  ::wtc_remove(WT_FACILITY_SCR);
  ::wtc_subscribe(WT_FACILITY_SCR, key_rearm, key_action, m_pasteboard);
  MouseSensor::instance().start(pasteboard());
  MouseSensor::instance().add(this,m_display);
  if ( xml ) {
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name,xml));
  }
  else if ( all ) {
    m_svc = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);
  }
  else {
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name));
  }
}

/// Standard destructor
FarmLineDisplay::~FarmLineDisplay()  {  
  MouseSensor::instance().stop();
  ::wtc_remove(WT_FACILITY_SCR);
  disconnect();
  m_listener = auto_ptr<PartitionListener>(0);
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_ctrlDisplay = auto_ptr<CtrlNodeDisplay>(0);
  m_mbmDisplay = auto_ptr<BufferDisplay>(0);
  if ( m_nodeSelector ) {
    MouseSensor::instance().remove(m_nodeSelector->display());
    m_nodeSelector = 0;
  }
  if ( m_subfarmDisplay ) {
    MouseSensor::instance().remove(m_subfarmDisplay->display());
    m_subfarmDisplay->finalize();
    delete m_subfarmDisplay;
    m_subfarmDisplay = 0;
  }
  subDisplays().clear();
  close();
  ::scrc_end_pasteboard_update (m_pasteboard);
  ::scrc_delete_pasteboard(m_pasteboard);
  m_pasteboard = 0;
  ::scrc_resetANSI();
  ::printf("Farm display deleted and resources freed......\n");
}

/// DIM command service callback
void FarmLineDisplay::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmLineDisplay* disp = *(FarmLineDisplay**)tag;
    disp->update(address);
  }
}

/// Get the name of the currently selected cluster
string FarmLineDisplay::selectedCluster() const {
  if ( m_sysDisplay.get() )
    return m_sysDisplay->clusterName();
  else if ( m_subfarmDisplay )
    return m_subfarmDisplay->clusterName();
  else if ( currentDisplay() )
    return currentDisplay()->name();
  return "";
}

/// Get the name of the currently selected cluster and node
pair<string,string> FarmLineDisplay::selectedNode() const {
  string node_name, cl = selectedCluster();
  if ( !cl.empty() ) {
    if ( m_sysDisplay.get() )
      node_name = m_sysDisplay->nodeName(m_subPosCursor);
    else if ( m_subfarmDisplay )
      node_name = m_subfarmDisplay->nodeName(m_subPosCursor);
  }
  return make_pair(cl,node_name);
}

/// Number of sub-nodes in a cluster
size_t FarmLineDisplay::selectedClusterSize() const {
  if ( m_sysDisplay.get() )
    return m_sysDisplay->numNodes();
  else if ( m_subfarmDisplay )
    return m_subfarmDisplay->numNodes();
  return 0;
}

/// Keyboard rearm action
int FarmLineDisplay::key_rearm (unsigned int /* fac */, void* param)  {
  Pasteboard* pb = (Pasteboard*)param;
  return ::scrc_fflush(pb);
}

/// Keyboard action
int FarmLineDisplay::key_action(unsigned int /* fac */, void* /* param */)  {
  int key = ::scrc_read_keyboard(0,0);
  if (!key) return WT_SUCCESS;
  return s_fd->handleKeyboard(key);
}

/// Set cursor to position
void FarmLineDisplay::set_cursor() {
  if ( 0 != m_sysDisplay.get() ) {
    Display* d1 = m_sysDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else if ( 0 != m_subfarmDisplay ) {
    Display* d1 = m_subfarmDisplay->display();
    if ( d1 ) ::scrc_set_cursor(d1, m_subPosCursor+8, 2); // 8 is offset in child window to select nodes
  }
  else {
    for(SubDisplays::iterator k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
      ClusterLine* curr = (*k).second;
      if ( curr == m_currentLine ) {
	::scrc_put_chars(m_display,curr->name().c_str(),NORMAL|BOLD,curr->position(),CLUSTERLINE_START,0);
      }
    }
    for(SubDisplays::iterator k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k) {
      ClusterLine* curr = (*k).second;
      if ( curr->position() == m_posCursor+CLUSTERLINE_FIRSTPOS ) {
	m_currentLine = curr;
	::scrc_put_chars(m_display,curr->name().c_str(),BLUE|BOLD|INVERSE,curr->position(),CLUSTERLINE_START,0);
      }
    }
  }
}

/// Get farm display from cursor position
ClusterLine* FarmLineDisplay::currentDisplay()  const {
  return m_currentLine;
}

/// Get farm display name from cursor position
string FarmLineDisplay::currentDisplayName()  const {
  ClusterLine* d = currentDisplay();
  return d ? d->name() : string("");
}

/// DIM command service callback
void FarmLineDisplay::update(const void* address) {
  char *msg = (char*)address;
  string svc, node;
  size_t idx, idq;
  switch(msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    if ( idq == string::npos ) idq = svc.find("/mona");
    if ( idq == string::npos ) idq = svc.find("/store");
    if ( idx != string::npos && idq == 0 ) {
      string f = svc.substr(1,idx-1);
      if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
        IocSensor::instance().send(this,CMD_ADD,new string(f));
      }
    }
    break;
  case '-':
    break;
  case '!':
    //getServiceNode(++msg,svc,node);
    //log() << "Service " << msg << " in ERROR." << endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      auto_ptr<Farms> farms(new Farms);
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
        last = strchr(at,'|');
        if ( last ) *last = 0;
        getServiceNode(p,svc,node);
        idx = svc.find("/ROpublish");
        idq = svc.find("/hlt");
        if ( idq == string::npos ) idq = svc.find("/mona");
        if ( idq == string::npos ) idq = svc.find("/store");
        if ( idx != string::npos && idq == 0 ) {
          string f = svc.substr(1,idx-1);
          if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
            farms->push_back(f);
          }
        }
        p = last+1;
      }
      if ( !farms->empty() )
        IocSensor::instance().send(this,CMD_CONNECT,farms.release());
    }
    break;
  }
}

/// Handle keyboard interrupts
int FarmLineDisplay::handleKeyboard(int key)    {
  if ( FarmDisplayBase::handleKeyboard(key) == WT_SUCCESS ) {
    return WT_SUCCESS;
  }
  ClusterLine* d = 0;
  RTL::Lock lock(screenLock());
  try {
    switch (key)    {
    case MOVE_UP:
      if( 0 == m_nodeSelector && m_posCursor > 0 )
        --m_posCursor;
      else if( m_nodeSelector && int(m_subPosCursor) >= 0 )
        --m_subPosCursor;
      break;
    case MOVE_DOWN:
      if( 0 == m_nodeSelector && m_posCursor < subDisplays().size()-1 )
        ++m_posCursor;
      else if( m_nodeSelector && selectedClusterSize() > m_subPosCursor )
        ++m_subPosCursor;
      break;
    case MOVE_LEFT:
      if( 0 == m_subfarmDisplay && 0 == m_sysDisplay.get() && (d=currentDisplay()) ) {
      }
      break;
    case MOVE_RIGHT:
      if( 0 == m_subfarmDisplay && 0 == m_sysDisplay.get() && (d=currentDisplay()) ) {
      }
      break;
    default:
      return WT_SUCCESS;
    }
  }
  catch(...) {
  }
  set_cursor();
  return WT_SUCCESS;
}

/// Interactor overload: Display callback handler
void FarmLineDisplay::handle(const Event& ev) {
  int cnt = 0;
  time_t now = time(0);
  ClusterLine* d = 0;
  Farms::iterator i;
  SubDisplays::iterator k;
  const MouseEvent* m = 0;

  RTL::Lock lock(screenLock());
  switch(ev.eventtype) {
  case ScrMouseEvent:
    m = ev.get<MouseEvent>();
    if ( handleMouseEvent(m) ) {
    }
    else if ( 0 == m_nodeSelector )    {
      m_posCursor = m->y - (CLUSTERLINE_FIRSTPOS + 1);
      set_cursor();
    }
    else if ( m_nodeSelector && m->msec == (unsigned int)-1 ) {
      Display* disp = m_nodeSelector->display();
      if ( m->display == disp )    {
        size_t pos = m->y - disp->row - 2;
        if ( selectedClusterSize()>pos ) {
          int cmd = m->button==0 ? m_sysDisplay.get() ? CMD_SHOWCTRL : CMD_SHOWMBM : CMD_SHOWPROCS;
          m_subPosCursor = pos;
          IocSensor::instance().send(this,cmd,this);
        }
      }
    }
    return;
  case TimeEvent:
    if (ev.timer_data == m_subfarmDisplay ) {
      IocSensor::instance().send(this,CMD_UPDATE,this);
    }
    return;
  case IocEvent:
    if ( handleIocEvent(ev) ) {
      return;
    }
    switch(ev.type) {
    case CMD_SETCURSOR:
      set_cursor();
      break;
    case CMD_HANDLE_KEY:
      handleKeyboard(int((long)ev.data));
      break;
    case CMD_SHOW:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
        if ( (d=(*k).second) == ev.data )  {
          m_posCursor = cnt;
          IocSensor::instance().send(this,CMD_SHOWSUBFARM,this);
          return;
        }
      }
      break;
    case CMD_POSCURSOR:
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k, ++cnt) {
        if ( (d=(*k).second) == ev.data )  {
          m_posCursor = cnt;
          set_cursor();
          return;
        }
      }
      break;
    case CMD_UPDATE:
      if ( m_subfarmDisplay )   {
        IocSensor::instance().send(m_subfarmDisplay,  ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_sysDisplay.get() )   {
        IocSensor::instance().send(m_sysDisplay.get(),ROMonDisplay::CMD_UPDATEDISPLAY,this);
      }
      if ( m_mbmDisplay.get() )  {
        const void* data = m_subfarmDisplay->data().pointer;
        m_mbmDisplay->setNode(m_subPosCursor);
        m_mbmDisplay->update(data);
      }
      else if ( m_mode == CTRL_MODE && m_ctrlDisplay.get() ) {
        const void* data = m_subfarmDisplay->data().pointer;
        m_ctrlDisplay->setNode(m_subPosCursor);
        m_ctrlDisplay->update(data);
      }
      TimeSensor::instance().add(this,1,m_subfarmDisplay);
      break;
    case CMD_ADD:
      if ( (i=find(m_farms.begin(),m_farms.end(),*ev.iocPtr<string>())) == m_farms.end() ) {
        m_farms.push_back(*ev.iocPtr<string>());
        connect(m_farms);
      }
      delete ev.iocPtr<string>();
      return;
    case CMD_CONNECT:
      m_farms = *ev.iocPtr<StringV>();
      connect(m_farms);
      delete ev.iocPtr<StringV>();
      return;
    case CMD_CHECK: {
      DisplayUpdate update(this,true);
      for(k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
        if ( (d=(*k).second) != ev.data ) d->check(now);
      if ( m_sysDisplay.get() ) m_sysDisplay->update();
      break;
    }
    case CMD_NOTIFY: {
      unsigned char* ptr = (unsigned char*)ev.data;
      if ( ptr ) {
	if ( m_benchDisplay.get() ) m_benchDisplay->update(ptr + sizeof(int), *(int*)ptr);
	delete [] ptr;
      }
      return;
    }
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void FarmLineDisplay::connect(const vector<string>& vfarms) {
  typedef set<string> FarmSet;
  SubDisplays::iterator k;
  SubDisplays copy;
  char txt[256];
  DisplayUpdate update(this,false);

  set<string> farms;
  for (Farms::const_iterator v=vfarms.begin(); v != vfarms.end(); ++v) farms.insert(*v);

  ::sprintf(txt,"Total number of subfarms:%d    ",int(farms.size()));
  ::scrc_put_chars(m_display,txt,NORMAL|BOLD,2,3,0);
  if ( m_mode == CTRL_MODE ) {
    ::sprintf(txt," %-10s %-8s%-7s %-6s %-6s    %s",
	      "","Last","No.of","No.of","No.of","< -------- Subfarm Information ------- >");
    ::scrc_put_chars(m_display,txt,INVERSE|BLUE,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s   %7s %5s  %-19s           %s",
	      "","Update","Nodes", "Tasks","Conn.","Status","PVSS","Summary",
	      "Controls PC and worker node status");
    ::scrc_put_chars(m_display,txt,INVERSE|BLUE,CLUSTERLINE_FIRSTPOS-1,1,1);
  }
  else {
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s   %35s%44s%47s",
	      "","Last","No.of","No.of","Num.of","",
	      "<<---------- Summ Counters --------->>",
	      "<<--------- Minimum Counters -------->>");
    ::scrc_put_chars(m_display,txt,INVERSE|BLUE,CLUSTERLINE_FIRSTPOS-2,1,1);
    ::sprintf(txt," %-10s %-8s %-6s %-6s %-6s   %-32s  %9s%5s%11s%6s%9s%5s  %9s%5s%10s%7s%9s%5s",
	      "Subfarm","Update","Nodes", "Buffer","Client","Subfarm status",
	      "MEP","Sl","EVENT","Sl","SEND","Sl","MEP","Sl","EVENT","Sl","SEND","Sl");
    ::scrc_put_chars(m_display,txt,INVERSE|BLUE,CLUSTERLINE_FIRSTPOS-1,1,1);
  }
  m_currentLine = 0;

  int pos = CLUSTERLINE_FIRSTPOS-1;
  for (FarmSet::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmDisplays.find(*i);
    ++pos;
    if ( k == m_farmDisplays.end() ) {
      if ( m_mode == RECO_MODE )
        copy.insert(make_pair(*i,createRecFarmClusterLine(this,pos,*i)));
      else if ( m_mode == CTRL_MODE )
        copy.insert(make_pair(*i,createCtrlFarmClusterLine(this,pos,*i)));
      else if ( ::strncasecmp((*i).c_str(),"mona0",5)==0 )
        copy.insert(make_pair(*i,createMonitoringClusterLine(this,pos,*i)));
      else if ( ::strncasecmp((*i).c_str(),"storectl",8)==0 )
        copy.insert(make_pair(*i,createStorageClusterLine(this,pos,*i)));
      else
        copy.insert(make_pair(*i,createFarmClusterLine(this,pos,*i)));
    }
    else {
      copy.insert(*k);
      m_farmDisplays.erase(k);
      continue;
    }
    if ( !m_currentLine ) m_currentLine = copy[*i];
  }
  for (k=m_farmDisplays.begin(); k != m_farmDisplays.end(); ++k)
    delete (*k).second;
  m_farmDisplays = copy;
}

static size_t do_output(void*,int,const char* fmt, va_list args) {
  char buffer[1024];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  return len;
}

extern "C" int romon_farmex(int argc,char** argv) {
  FarmLineDisplay* disp = new FarmLineDisplay(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
