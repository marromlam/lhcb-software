// $Id: TTHitExpectation.cpp,v 1.11 2010-04-07 21:51:06 wouter Exp $

// from GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "TsaKernel/Line3D.h"
#include "LHCbMath/GeomFun.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSensor.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/StateParameters.h"

#include "TTHitExpectation.h"
#include <boost/foreach.hpp>
#include <algorithm>

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TTHitExpectation );
//=============================================================================
// 
//=============================================================================
TTHitExpectation::TTHitExpectation(const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent):
  GaudiTool(type, name, parent),
  m_selector(0)
{ 
  // constructer

  declareProperty("extrapolatorName", m_extrapolatorName = "TrackParabolicExtrapolator");
  declareProperty( "SelectorType", m_selectorType = "STSelectChannelIDByElement" );
  declareProperty( "SelectorName", m_selectorName = "ALL" ); 
  declareProperty( "allStrips", m_allStrips = false);

  declareInterface<IHitExpectation>(this);
};

//=============================================================================
// 
//=============================================================================
TTHitExpectation::~TTHitExpectation(){
  // destructer
}

//=============================================================================
// 
//=============================================================================
StatusCode TTHitExpectation::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

 
  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);
  m_ttDet = getDet<DeSTDetector>(DeSTDetLocation::location("TT"));

  if (m_ttDet->nStation() != 2u){
    return Error("2 stations needed in TT",StatusCode::FAILURE);
  }
 
  const DeSTDetector::Stations& ttStations = m_ttDet->stations();
  m_zTTa = ttStations[0]->globalCentre().z();
  m_zTTb = ttStations[1]->globalCentre().z();

  // (selector
  if (m_selectorName != "ALL")
    m_selector  = tool< ISTChannelIDSelector >( m_selectorType,m_selectorName );

  return StatusCode::SUCCESS;
}


IHitExpectation::Info TTHitExpectation::expectation(const LHCb::Track& aTrack) const{

  IHitExpectation::Info info;
  info.likelihood = 0.0;
  info.nFound = 0;
  info.nExpected = nExpected(aTrack);
  return info;
}

unsigned int TTHitExpectation::nExpected(const LHCb::Track& aTrack) const{

  // make a line at TTa and TTb
  const State& TTaState = aTrack.closestState(m_zTTa);
  StateVector stateVectorTTa(TTaState.position(), TTaState.slopes());

  const State& TTbState = aTrack.closestState(m_zTTb);
  StateVector stateVectorTTb(TTbState.position(), TTbState.slopes());

  // determine which modules should be hit
  std::vector<STChannelID> expectedHitsA; expectedHitsA.reserve(4);
  std::vector<STChannelID> expectedHitsB; expectedHitsB.reserve(4);
  collectHits(expectedHitsA, stateVectorTTa, 1);
  collectHits(expectedHitsB, stateVectorTTb, 2);

  return expectedHitsA.size() + expectedHitsB.size();  
}

void TTHitExpectation::collect(const LHCb::Track& aTrack, std::vector<LHCb::LHCbID>& ids) const{

  // make a line at TTa and TTb
  const State& TTaState = aTrack.closestState(m_zTTa);
  StateVector stateVectorTTa(TTaState.position(), TTaState.slopes());

  const State& TTbState = aTrack.closestState(m_zTTb);
  StateVector stateVectorTTb(TTbState.position(), TTbState.slopes());

  // determine which modules should be hit
  std::vector<STChannelID> expectedHits; expectedHits.reserve(8);
  collectHits(expectedHits, stateVectorTTa, 1);
  collectHits(expectedHits, stateVectorTTb, 2);

  // convert to LHCb ids
  ids.reserve(expectedHits.size());
  BOOST_FOREACH(STChannelID chan, expectedHits) {
    ids.push_back(LHCbID(chan));
  }
 
} 

void TTHitExpectation::collectHits(std::vector<LHCb::STChannelID>& chans, 
                                  LHCb::StateVector stateVec, const unsigned int station ) const{

  // loop over the sectors
  const DeSTDetector::Sectors& sectorVector = m_ttDet->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectorVector.begin();
  for (; iterS != sectorVector.end(); ++iterS){
    // propagate to z of sector
    m_extrapolator->propagate(stateVec,(*iterS)->globalCentre().z());    
    const STChannelID elemID = (*iterS)->elementID();
    if (elemID.station() == station){
      // loop over sensors
      const DeSTSector::Sensors& tsensors = (*iterS)->sensors();
      DeSTSector::Sensors::const_iterator iter = tsensors.begin();
      for (; iter != tsensors.end(); ++iter){
        Tf::Tsa::Line3D aLine3D(stateVec.position(), stateVec.slopes());
        if (select((*iterS)->elementID()) && insideSensor(*iter, aLine3D) == true){

          // get the list of strips that could be in the cluster
          Gaudi::XYZPoint globalEntry = intersection(aLine3D,(*iter)->entryPlane());
          Gaudi::XYZPoint globalExit = intersection(aLine3D,(*iter)->exitPlane());
          Gaudi::XYZPoint localEntry = (*iter)->toLocal(globalEntry);
          Gaudi::XYZPoint localExit =  (*iter)->toLocal(globalExit);
          
          unsigned int firstStrip = (*iter)->localUToStrip(localEntry.x());
          unsigned int lastStrip =  (*iter)->localUToStrip(localExit.x());
          
          // might have to swap...
          if (firstStrip > lastStrip) std::swap(firstStrip, lastStrip);

          // allow for capacitive coupling....
          if ((*iter)->isStrip(firstStrip-1) == true) --firstStrip;
          if ((*iter)->isStrip(lastStrip+1) == true) ++lastStrip;
          
          bool found = false;
          unsigned int middleStrip = (firstStrip+lastStrip)/2;
          for (unsigned int iStrip = firstStrip ; iStrip != lastStrip; ++iStrip){        
            const STChannelID chan = (*iterS)->stripToChan(iStrip);
            if ((*iterS)->isOKStrip(chan) == true){ // check it is alive
              if (m_allStrips == true) {
                chans.push_back(chan); // take them all
              }
              else {
                found = true; // take just the sector
              }
            } // ok strip
          } // loop strips

          if (!m_allStrips && found == true) {
            STChannelID midChan(elemID.type(), elemID.station(), 
                                elemID.layer(), elemID.detRegion(), 
                                elemID.sector(), middleStrip);
            chans.push_back(midChan);
          }
        } // select
      }  // iter
    } // station
  } // sector
}


Gaudi::XYZPoint TTHitExpectation::intersection(const Tf::Tsa::Line3D& line,
                                            const Gaudi::Plane3D& aPlane) const{

  // make a plane
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}


