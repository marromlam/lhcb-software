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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/src/PartitionDisplay.cpp,v 1.4 2008-05-13 07:55:40 frankb Exp $

// Framework include files
#include "ROLogger/PartitionDisplay.h"
#include "ROLogger/ClusterDisplay.h"
#include "ROLogger/FilterDisplay.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include "ROLoggerDefs.h"
#include <sstream>
extern "C" {
  #include "dic.h"
}
using namespace ROLogger;

static char* s_show[]    = {"Show"};
static char* s_config[]  = {"Configure"};
static char* s_enable[]  = {"Enable "};
static char* s_disable[] = {"Disable"};
static char  s_enableDisableResult[80];
static char  s_param_buff[80];
static int   s_NumList[] = {1,10,50,100,200,300,500,1000,5000,10000};

/// Standard constructor
PartitionDisplay::PartitionDisplay(Interactor* parent, Interactor* msg, Interactor* history, const std::string& name) 
  : m_name(name), m_parent(parent), m_msg(msg), m_history(history), m_numMsg(200)
{
  m_id = UpiSensor::instance().newID();
  ::strcpy(m_wildNode,"*");
  ::strcpy(m_wildMessage,"*");
  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"Error logger",RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_add_comment(CMD_COM0,("Known processor clusters for partition "+name).c_str(),"");
  ::upic_add_comment(CMD_COM1,"---------------------------------------------------------","");
  ::upic_add_comment(CMD_COM2,"---------------------------------------------------------","");
  ::upic_set_param(&m_numMsg,1,"I6",m_numMsg,0,0,s_NumList,sizeof(s_NumList)/sizeof(s_NumList[0]),0);
  ::upic_add_command(CMD_COM3,         "Show                  ^^^^^^ Messages","");
  ::upic_set_param(m_wildNode,1,"A16",m_wildNode,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_NODE,    "Node match:           ^^^^^^^^^^^^^^^","");
  ::upic_set_param(m_wildMessage,1,"A16",m_wildMessage,0,0,0,0,0);
  ::upic_add_command(CMD_WILD_MESSAGE, "...and match message: ^^^^^^^^^^^^^^^","");
  ::upic_add_command(CMD_CLEAR_HISTORY,"Clear history","");
  ::upic_add_command(CMD_SUMM_HISTORY, "History summary","");
  ::upic_add_command(CMD_SHOW_FILTERS, "Edit filters","");
  ::upic_add_comment(CMD_COM5,"---------------------------------------------------------","");
  ::upic_add_command(CMD_CLOSE,"Close","");
  ::upic_close_menu();
  UpiSensor::instance().add(this,m_id);
}

/// Standard destructor
PartitionDisplay::~PartitionDisplay()  {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

/// Show processor cluster display
void PartitionDisplay::showCluster(int cmd) {
  Nodes nodes;
  dim_lock();
  const std::string& name = m_items[cmd].second;
  for(Nodes::const_iterator i=m_nodes.begin();i!=m_nodes.end();++i) {
    if ( strncmp((*i).c_str(),name.c_str(),name.length())==0 ) 
      nodes.push_back(*i);
  }
  new ClusterDisplay(this,m_history,name,nodes);
  m_menuCursor = cmd;
  dim_unlock();
}

/// Show history according to node and message pattern match
void PartitionDisplay::showHistory(const char* node_match, const char* msg_match) {
  std::stringstream s;
  s << "#Node:{" << node_match << "}#Msg:{" << msg_match << "}#Num:{" << m_numMsg << "}" << std::ends;
  IocSensor::instance().send(m_history,CMD_NODE_HISTORY,new std::string(s.str()));
}

static std::string setupParams(const std::string& name, bool val) {
  char text[132];
  ::sprintf(text,"%-12s %-10s  ^^^^     ^^^^^^^     ^^^^^^^^^",
	    name.c_str(), val ? "[ENABLED]" : "[DISABLED]");
  ::upic_set_param(s_param_buff,1,"A4",s_show[0],0,0,s_show,1,1);
  ::upic_set_param(s_enableDisableResult,2,"A7",val?s_disable[0]:s_enable[0],
		   0,0,val?s_disable:s_enable,1,1);
  ::upic_set_param(s_param_buff,3,"A9",s_config[0],0,0,s_config,1,1);
  return text;
}

/// Update farm content
void PartitionDisplay::updateFarms() {
  int nf = 0;
  for(Items::const_iterator i=m_items.begin(); i!=m_items.end(); ++i)
    ::upic_delete_command(m_id,(*i).first);
  nf = 0;
  m_items.clear();
  for(Farms::const_iterator f=m_farms.begin(); f!=m_farms.end(); ++f) {
    setupParams(*f,true);
    ::upic_insert_param_line(m_id, CMD_COM2, ++nf, setupParams(*f,true).c_str(), "");
    m_items[nf] = std::make_pair(true,(*f));
  }
}

void PartitionDisplay::handle(const Event& ev) {
  typedef std::vector<std::string> _SV;
  IocSensor& ioc = IocSensor::instance();
  switch(ev.eventtype) {
  case IocEvent:
    //::upic_write_message2("Got IOC command: %d %p",ev.type,ev.data);
    switch(ev.type) {
    case CMD_UPDATE_NODES:
      m_nodes = *(Nodes*)ev.data;
      delete (Nodes*)ev.data;
      ::upic_write_message2("Updating node content of %s [%ld nodes]",m_name.c_str(),m_nodes.size());
      return;
    case CMD_UPDATE_FARMS:
      m_farms = *(Farms*)ev.data;
      ::upic_write_message2("Updating farm content of %s [%ld nodes]",m_name.c_str(),m_farms.size());
      ioc.send(this,CMD_UPDATE_CLUSTERS,this);
      ioc.send(m_msg,CMD_UPDATE_FARMS,new _SV(*(_SV*)ev.data));
      ioc.send(m_history,CMD_UPDATE_FARMS,ev.data);
      return;
    case CMD_UPDATE_CLUSTERS:
      updateFarms();
      return;
    case CMD_DELETE:
      ::upic_set_cursor(m_id,m_menuCursor,0);
      delete (Interactor*)ev.data;
      break;
    default:
      break;
    }
    break;
  case UpiEvent:
    //::upic_write_message2("Got UPI command: %d %d %d",ev.menu_id,ev.command_id,ev.param_id);
    m_wildNode[sizeof(m_wildNode)-1] = 0;
    m_wildMessage[sizeof(m_wildMessage)-1] = 0;
    if ( ::strchr(m_wildNode,' ') ) *::strchr(m_wildNode,' ') = 0;
    if ( ::strchr(m_wildMessage,' ') ) *::strchr(m_wildMessage,' ') = 0;
    switch(ev.command_id) {
    case CMD_CLOSE:
      ioc.send(m_parent,CMD_DELETE,this);
      return;
    case CMD_SUMM_HISTORY:
      ioc.send(m_history,CMD_SUMM_HISTORY,CMD_SUMM_HISTORY);
      return;
    case CMD_WILD_NODE:
      showHistory(m_wildNode,"*");
      return;
    case CMD_WILD_MESSAGE:
      showHistory(m_wildNode,m_wildMessage);
      return;
    case CMD_CLEAR_HISTORY:
      IocSensor::instance().send(m_history,CMD_CLEAR_HISTORY,CMD_CLEAR_HISTORY);
      return;
    case CMD_SHOW_FILTERS:
      new FilterDisplay(this,m_msg,m_history);
      return;
    default:
      if ( ev.command_id > 0 && ev.command_id <= (int)m_farms.size() ) {
	int val, cmd = ev.command_id;
	switch(ev.param_id) {
	case 1:
	  showCluster(cmd);
	  break;
	case 2:
	  val = m_items[cmd].first = ::strcmp(s_enableDisableResult,s_enable[0])==0;
	  ::upic_replace_param_line(m_id,cmd,setupParams(m_items[cmd].second,val).c_str(),"");
	  ::upic_set_cursor(ev.menu_id,cmd,ev.param_id);
	  ioc.send(m_msg,val ? CMD_DISCONNECT_CLUSTER : CMD_CONNECT_CLUSTER,new std::string(m_items[cmd].second));
	  break;
	case 3:
	  ::upic_write_message("Configuration by menu not implemented...","");
	  break;
	default:
	  break;
	}
      }
    }
    break;
  default:  // Fall through: Handle request by client
    break;
  }
}
