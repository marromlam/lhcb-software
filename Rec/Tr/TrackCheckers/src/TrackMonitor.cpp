// $Id: TrackMonitor.cpp,v 1.2 2007-06-27 15:05:06 mneedham Exp $
// Include files 
#include "TrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/State.h"

// gsl
#include "gsl/gsl_math.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"

#include <map>

#include "Map.h"

DECLARE_ALGORITHM_FACTORY( TrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitor::TrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 0.0);
 
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitor::~TrackMonitor() {}; 

StatusCode TrackMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackMonitorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // guess that the size of the histograms at ref is ~ 0.35 by 0.3
  m_xMax = GSL_MAX(5.0*Gaudi::Units::cm, 0.35*m_refZ);
  m_yMax = GSL_MAX(5.0*Gaudi::Units::cm, 0.3*m_refZ);

  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackMonitor::execute()
{

  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  std::map<std::string, unsigned int> tMap;
  std::string type = "";

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0., 500., 50);

  // histograms per track
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  for (; iterT != tracks->end(); ++iterT){
    if (selector((*iterT)->type())->accept(**iterT) == true){

      splitByAlgorithm() == true ? 
        type = Gaudi::Utils::toString((*iterT)->history()) : type= all(); 

      tMap[type]+= 1;
      fillHistograms(*iterT,type);
      plot((*iterT)->type(),2, "type" ,-0.5, 10.5, 10);
    }
  } // iterT

  // fill counters....
  counter("#Tracks") += tracks->size();
  for (std::map<std::string,unsigned int>::const_iterator iterS = tMap.begin();
       iterS != tMap.end(); ++iterS){
    counter("#"+iterS->first) += iterS->second;
  } // iterS

  return StatusCode::SUCCESS;
};

void TrackMonitor::fillHistograms(const LHCb::Track* aTrack, 
                                  const std::string& type) {

  // plots we should always make...
  plot(aTrack->probChi2(),type+"/2","probChi2", -0.01, 1.01, 51);
  plot(aTrack->chi2PerDoF(),type+"/3","chi2/ndof", -2., 202., 51);
  plot(aTrack->nLHCbIDs(),type+"/4","#nLHCbIDs", -0.5, 60.5, 61);
  plot(aTrack->p()/Gaudi::Units::GeV, type+"/5" ,"momentum", -5., 205., 21);
  plot(aTrack->pt()/Gaudi::Units::GeV,type+"/6", "pt", -0.1, 10.1, 51);
  plot(aTrack->pseudoRapidity(),type+"/7", "eta", 0.95 , 6.05, 50);

  // expert checks  
  if (fullDetail() == true){
    plot(aTrack->nMeasurements(),type+"/100","#nMeas",  -0.5, 60., 61);
    plot(aTrack->nMeasurementsRemoved(),type+"/101","#outliers", -0.5, 10.5, 61);
   
    // track parameters at some reference z
    LHCb::State aState;
    StatusCode sc = extrapolator()->propagate(*aTrack, m_refZ,
                                              aState );
    plot2D(aState.x()/Gaudi::Units::cm,
           aState.y()/Gaudi::Units::cm, type+"/200", "x vs y", 
           -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
    plot(aState.tx(),type+"/103","tx", -1.,1., 50);
    plot(aState.ty(),type+"/104", "ty", -1.,1., 50);

    const LHCb::Track::ExtraInfo& info = aTrack->extraInfo();
    LHCb::Track::ExtraInfo::const_iterator iterInfo = info.begin();
    for (;iterInfo != info.end(); ++iterInfo ){
      LHCb::Track::AdditionalInfo infoName = 
      LHCb::Track::AdditionalInfo(iterInfo->first);
      std::string title = Gaudi::Utils::toString(infoName);
      const TrackMaps::InfoHistMap& histMap = TrackMaps::infoHistDescription();
      const TrackMaps::HistoRange range = histMap.find(infoName)->second;
      plot(iterInfo->second,type+"/info/"+range.fid, title,range.fxMin ,range.fxMax , 100);
    } // iterInfo

  }
}

