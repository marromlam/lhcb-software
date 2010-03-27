// $Id: HltGlobalMonitor.cpp,v 1.63 2010-03-27 20:07:53 graven Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// BOOST
// ============================================================================
#include "boost/assign/list_of.hpp"
#include "boost/algorithm/string/erase.hpp"

// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/HistoStats.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Memory Usage
// ============================================================================
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/Memory.h"
// ============================================================================
// HLT
// ============================================================================
#include "HltBase/HltHistogramUtilities.h"
using namespace Hlt::HistogramUtilities;
// ============================================================================
// local
// ============================================================================
#include "HltGlobalMonitor.h"
// ============================================================================

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor );

// utilities


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_currentTime(0)
  , m_timeSize(0)
  , m_timeInterval(0)
  , m_startClock(0)
  , m_startEvent(0)
  , m_virtmem(0)
  , m_gpstimesec(0)
  , m_time_ref(0)
  , m_totaltime(0)
  , m_totalmem(0)
{
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("HltDecReports",     m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt1Decisions",     m_Hlt1Lines );
  declareProperty("Hlt2Decisions",     m_Hlt2Lines );
  declareProperty("TotalMemory",       m_totalmem   = 3000 );
  declareProperty("TimeSize",          m_timeSize = 120 );   // number of minutes of history (half an hour)
  declareProperty("TimeInterval",      m_timeInterval = 1 ); // binwidth in minutes 
  declareProperty("DecToGroupHlt1",    m_DecToGroup1);
  declareProperty("DecToGroupHlt2",    m_DecToGroup2);
}
//=============================================================================
// Destructor
//=============================================================================
HltGlobalMonitor::~HltGlobalMonitor() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGlobalMonitor::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_odin            = book1D("ODIN trigger type",  "ODIN trigger Type ",-0.5, 7.5, 8);
  std::vector<std::pair<unsigned,std::string> > odinLabels = boost::assign::list_of< std::pair<unsigned,std::string> >
    (ODIN::PhysicsTrigger,    "Physics")
    (ODIN::BeamGasTrigger,    "BeamGas")
    (ODIN::LumiTrigger,       "Lumi")
    (ODIN::TechnicalTrigger,  "Technical")
    (ODIN::AuxiliaryTrigger,  "Auxiliary")
    (ODIN::NonZSupTrigger,    "NonZSup")
    (ODIN::TimingTrigger,     "Timing")
    (ODIN::CalibrationTrigger,"Calibration");
  if (!setBinLabels( m_odin, odinLabels )) {
    error() << "failed to set binlables on ODIN hist" << endmsg;
  }

  m_gpstimesec=0;
  m_startClock = System::currentTime( System::microSec );
  // create a histogram with one bin per Alley
  // the order and the names for the bins are
  // configured in HLTConf/Configuration.py  

  std::vector<std::string> hlt1AlleyLabels;
  for (DecToGroupType::const_iterator i = m_DecToGroup1.begin(); i!=m_DecToGroup1.end(); ++i ) {
      hlt1AlleyLabels.push_back(i->first);
  }

  m_hlt1Alley       = book1D("Hlt1 Alleys", "Hlt1 Alleys", -0.5, hlt1AlleyLabels.size()-0.5 , hlt1AlleyLabels.size() );
  if (!setBinLabels( m_hlt1Alley, hlt1AlleyLabels )) {
    error() << "failed to set binlables on Hlt1 Alley hist" << endmsg;
  }

  std::vector<std::string> hlt2AlleyLabels;
  for (DecToGroupType::const_iterator i = m_DecToGroup2.begin(); i!=m_DecToGroup2.end(); ++i ) {
      hlt2AlleyLabels.push_back(i->first);
  }
  m_hlt2Alley       = book1D("Hlt2 Alleys", "Hlt2 Alleys", -0.5, hlt2AlleyLabels.size()-0.5 , hlt2AlleyLabels.size() );
  if (!setBinLabels( m_hlt2Alley, hlt2AlleyLabels )) {
    error() << "failed to set binlables on Hlt2 Alley hist" << endmsg;
  } 

  m_hlt1AlleysCorrelations      = book2D("Hlt1Alleys Correlations", -0.5, hlt1AlleyLabels.size()-0.5, hlt1AlleyLabels.size()
                                                                  , -0.5, hlt1AlleyLabels.size()-0.5, hlt1AlleyLabels.size() );
  if (!setBinLabels( m_hlt1AlleysCorrelations, hlt1AlleyLabels, hlt1AlleyLabels )) {
    error() << "failed to set binlables on Hlt1Alleys Correlation hist" << endmsg;
  }

  m_hlt2AlleysCorrelations      = book2D("Hlt2Alleys Correlations", -0.5, hlt2AlleyLabels.size()-0.5, hlt2AlleyLabels.size()
                                                                  , -0.5, hlt2AlleyLabels.size()-0.5, hlt2AlleyLabels.size() );
  if (!setBinLabels( m_hlt2AlleysCorrelations, hlt2AlleyLabels, hlt2AlleyLabels )) {
    error() << "failed to set binlables on Hlt2Alleys Correlation hist" << endmsg;
  }

                    /*One Histogram for each alley*/
  for(DecToGroupType::const_iterator i=m_DecToGroup1.begin();i!=m_DecToGroup1.end();++i){
    std::string alleyName = std::string("Hlt1 ")+i->first+" Lines";
    m_hlt1Alleys.push_back( book1D(alleyName,   -0.5,i->second.size()-0.5,i->second.size()) );
    m_hlt1AlleyRates.push_back(&counter(alleyName));
    declareInfo("COUNTER_TO_RATE["+alleyName+"]", *m_hlt1AlleyRates.back(),alleyName);
    std::vector<std::string> strip; 
    strip.push_back("Decision"); // and of course 'Decision'...
    strip.push_back("Hlt1");   // in case the above doesn't work, just strip 'Hlt1' 
    strip.push_back(i->first); // first try to remove alley prefix
    std::vector<std::string> labels;
    for (std::vector<std::string>::const_iterator j=i->second.begin();j!=i->second.end();++j) {
      std::string s = *j;
      for (std::vector<std::string>::const_iterator k = strip.begin();k!=strip.end();++k) {
        if (s!=*k) boost::algorithm::erase_all(s,*k);
      }
      labels.push_back(s);
      m_hlt1Line2AlleyBin[ *j ] =  std::make_pair( m_hlt1Alleys.size()-1, labels.size()-1 );
    }
    if (!setBinLabels( m_hlt1Alleys.back(),labels )) {
          error() << "failed to set binlables on Hlt1 " << i->first << " Alley hist" << endmsg;
    }
  }

  // for hlt2
  for(DecToGroupType::const_iterator i=m_DecToGroup2.begin();i!=m_DecToGroup2.end();++i){
    std::string alleyName = std::string("Hlt2 ")+i->first+" Lines";
    m_hlt2Alleys.push_back( book1D(alleyName, -0.5,i->second.size()-0.5,i->second.size()) );
    m_hlt2AlleyRates.push_back(&counter(alleyName));
    declareInfo("COUNTER_TO_RATE["+alleyName+"]", *m_hlt2AlleyRates.back(),alleyName);
    std::vector<std::string> strip; 
    strip.push_back("Decision"); // always remove 'Decision'...
    strip.push_back("Hlt2");   // and Hlt2
    strip.push_back(i->first); // finally try to remove alley prefix
    std::vector<std::string> labels;
    for (std::vector<std::string>::const_iterator j=i->second.begin();j!=i->second.end();++j) {
      std::string s = *j;
      for (std::vector<std::string>::const_iterator k = strip.begin();k!=strip.end();++k) {
        if (s!=*k) boost::algorithm::erase_all(s,*k);
      }
      labels.push_back(s);
      m_hlt2Line2AlleyBin[ *j ] =  std::make_pair( m_hlt2Alleys.size()-1, labels.size()-1 );
    }
    if (!setBinLabels( m_hlt2Alleys.back(),labels )) {
          error() << "failed to set binlables on Hlt2 " << i->first << " Alley hist" << endmsg;
    }
  }


  m_hltVirtTime  = bookProfile1D("Virtual memory",   0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
  setAxisLabels( m_hltVirtTime, "time since start of run [min]", "memory[MB]");
  
  m_hltEventsTime  = bookProfile1D("average time per event", 0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
  setAxisLabels( m_hltEventsTime, "time since start of run [min]", "average time/event [ms]");

  m_hltTime  = book1D("time per event ", -1, 4 );
  setAxisLabels( m_hltTime, "log10(time/event/ms)", "events");

  declareInfo("COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory");
  declareInfo("COUNTER_TO_RATE[elapsed time]", m_currentTime, "Elapsed time");
  

  //declareInfo("COUNTER_TO_RATE[L0Accept]",counter("L0Accept"),"L0Accept");
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"Gps time of last event");

  // register for incidents...
  IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
  // insert at high priority, so we also time what happens during BeginEvent incidents...
  long priority = std::numeric_limits<long>::max();
  incidentSvc->addListener(this,IncidentType::BeginEvent, priority ) ;
  incidentSvc->addListener(this,IncidentType::BeginRun, 0 ) ;

  // start by kicking ourselves in action -- just in case we don't get one otherwise...
  this->handle(Incident(name(), IncidentType::BeginRun ));

  return StatusCode::SUCCESS;
};

void HltGlobalMonitor::handle ( const Incident& incident ) {
  m_startEvent = System::currentTime( System::microSec );
  if (m_startClock == 0 || incident.type() == IncidentType::BeginRun) m_startClock = m_startEvent;
  m_currentTime = double(m_startEvent-m_startClock)/1000000 ;  // seconds
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitor::execute() {  

  LHCb::HltDecReports* hlt = fetch<LHCb::HltDecReports>( m_HltDecReportsLocation );
  LHCb::ODIN*         odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
 
  monitorODIN(odin);
  monitorHLT1(hlt);
  monitorHLT2(hlt);
  monitorMemory();
  
  counter("#events")++;

  return StatusCode::SUCCESS;
  
}

//==============================================================================
void HltGlobalMonitor::monitorODIN(const LHCb::ODIN* odin) {
  if (odin == 0 ) return;
  unsigned long long gpstime=odin->gpsTime();
  if (msgLevel(MSG::DEBUG)) debug() << "gps time" << gpstime << endreq;
  m_gpstimesec=int(gpstime/1000000-904262401); //@TODO: is this still OK with ODIN v6?
  counter("ODIN::Lumi")    += (odin->triggerType()==ODIN::LumiTrigger);
  counter("ODIN::NotLumi") += (odin->triggerType()!=ODIN::LumiTrigger);
  fill(m_odin, odin->triggerType(), 1.);
  return;
}

//==============================================================================
void HltGlobalMonitor::monitorHLT1(const LHCb::HltDecReports* hlt) {


  if (hlt==0) return;

  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<Gaudi::StringKey,const LHCb::HltDecReport*> > reps; reps.reserve(m_Hlt1Lines.size());
  unsigned nAcc = 0;
  std::vector<unsigned> nAccAlley(m_hlt1Alleys.size(),unsigned(0));

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();i++) {
    //TODO: augment HltDecReport to avoid doing 'find'
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
       continue;
    }
    Gaudi::StringKey key(*i);
    reps.push_back( std::make_pair( key, report ) );
    if (report && report->decision()){
      ++nAcc;
      std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> >::const_iterator j = m_hlt1Line2AlleyBin.find(key);
      if (j!=m_hlt1Line2AlleyBin.end()) {
          assert(j->second.first<nAccAlley.size());
          ++nAccAlley[ j->second.first ];
      }
    }
  }

  for (size_t i = 0; i<reps.size();++i) {
    bool accept = (reps[i].second->decision()!=0);
    if (!accept) continue;

           //filling the histograms for each alley
    std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> >::const_iterator k = m_hlt1Line2AlleyBin.find( reps[i].first );
    if (k!=m_hlt1Line2AlleyBin.end()) fill( m_hlt1Alleys[k->second.first], k->second.second, accept );
  }
       //filling the histograms for the alleys instead of the lines
  for (unsigned i=0; i<m_DecToGroup1.size();i++) {
    *m_hlt1AlleyRates[i] += ( nAccAlley[i] > 0 );
    fill(m_hlt1Alley,i,(nAccAlley[i]>0));
    if(nAccAlley[i]==0) continue;
    for(unsigned j=0; j<m_DecToGroup1.size();j++){
      fill(m_hlt1AlleysCorrelations,i,j,(nAccAlley[j]>0));
    }
  }

}

//==============================================================================

void HltGlobalMonitor::monitorHLT2(const LHCb::HltDecReports* hlt) {
  if (hlt==0) return;

  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<Gaudi::StringKey,const LHCb::HltDecReport*> > reps; reps.reserve(m_Hlt2Lines.size());
  unsigned nAcc = 0;
  std::vector<unsigned> nAccAlley(m_DecToGroup2.size(),unsigned(0));

  for (std::vector<std::string>::const_iterator i = m_Hlt2Lines.begin(); i!=m_Hlt2Lines.end();++i) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {
       warning() << "report " << *i << " not found" << endreq;
       continue;
    }
    Gaudi::StringKey key(*i);
    reps.push_back( std::make_pair( key, report ) );
    if (report && report->decision()){
      ++nAcc;
      std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> >::const_iterator j = m_hlt2Line2AlleyBin.find(key);
      if (j!=m_hlt2Line2AlleyBin.end()) {
          assert(j->second.first<nAccAlley.size());
          ++nAccAlley[ j->second.first ];
      }
    }
  }

  for (size_t i = 0; i<reps.size();++i) {
    bool accept = reps[i].second->decision();
    if (!accept) continue;
    
    // filling the histograms for each alley
    std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> >::const_iterator k = m_hlt2Line2AlleyBin.find(reps[i].first);
    if (k!=m_hlt2Line2AlleyBin.end()) fill( m_hlt2Alleys[k->second.first], k->second.second, accept );

  }
  //filling the histograms for the alleys instead of the lines

  for (unsigned i=0; i<m_DecToGroup2.size();i++) {
    *m_hlt2AlleyRates[i] += ( nAccAlley[i] > 0 );
    fill(m_hlt2Alley,i,(nAccAlley[i]>0));
    if(nAccAlley[i]==0) continue;
    for(unsigned j=0; j<m_DecToGroup2.size();j++){
      fill(m_hlt2AlleysCorrelations,i,j,(nAccAlley[j]>0));
    }
  }

}

//==============================================================================

void HltGlobalMonitor::monitorMemory() {

  double elapsedTime = double(System::currentTime( System::microSec ) - m_startEvent);
  double t = log10(elapsedTime)-3; // convert to log(time/ms)
  fill( m_hltTime, t ,1.0); 
  storeTrend(m_hltEventsTime, elapsedTime/1000 );

  //TODO: only need to do this once 'per bin'  interval...
  m_virtmem  = virtualMemory(System::MByte, System::Memory);
  storeTrend(m_hltVirtTime, double(m_virtmem));

 
}

//=============================================================================
void HltGlobalMonitor::storeTrend(AIDA::IProfile1D* h, double Value) 
{
#ifdef TODO_IMPLEMENT_SHIFT_USING_AIDA2ROOT_ON_A_PROFILE_HISTOGRAM
  double offset = m_currentTime - m_trendLHSEdge;
  if ( offset > ... ) {
      const AIDA::IAxis & axis = h->axis();
      long bins = axis.bins();
      for ( long i = 0; i < bins; ++i ) {
        double binValue = h->binHeight(i);
        double nextValue = ( i < bins - 1 ) ? h->binHeight(i+1)
                                            : Value;
        double x = 0.5*(axis.binUpperEdge(i)+axis.binLowerEdge(i));
        h->fill(x, nextValue - binValue);
      }
       update m_trendLHSEdge
       shift bins to left
  }
#endif
  h->fill(m_currentTime/60, Value); // go from seconds -> minutes
}
