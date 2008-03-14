// $Id: ITExpectedHits.cpp,v 1.3 2008-03-14 18:21:37 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "ITExpectedHits.h"

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( ITExpectedHits );

ITExpectedHits::ITExpectedHits(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  GaudiTool(type, name, parent)
{
  // constructer
  declareInterface<IITExpectedHits>(this);
}

ITExpectedHits::~ITExpectedHits(){
  // destructer
}

StatusCode ITExpectedHits::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  // get geometry
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location("IT"));

  return StatusCode::SUCCESS;
}

StatusCode ITExpectedHits::collect(const Parabola& parab,
                                   const Line& line,
                                   const LHCb::STChannelID& aChan,
                                   std::vector<IITExpectedHits::ITPair>& hits,
                                   const unsigned int iSector) const
{

  hits.reserve(8);

  unsigned int ibox = 0;
  if (iSector == 0){
    const double xTest = parab.value(TsaConstants::beginTracker);
    xTest < 0 ? ibox = 1 : ibox = 2;
  }
  else if (iSector == 1) {
    ibox = 3;
  }
  else {
    ibox = 4;
  }


  unsigned int iCount = 0;

  LHCb::STChannelID layerID = LHCb::STChannelID(aChan.type(),aChan.station(),aChan.layer(),ibox, 0u, 0u);

  const DeSTLayer* layer = m_tracker->findLayer(layerID);

  verbose() << "iSector = " << iSector << " ibox=" << ibox << " layerID=" << (int)layerID << " layer=" << layer << endreq;

  if (layer != 0){

    Line tanLine = parab.tangent(layer->globalCentre().z());

    Line3D aLine3D = createLine3D(tanLine,
                                  line,layer->globalCentre().z());

    const DeSTLayer::Sectors& tSectors = layer->sectors();
    for ( DeSTLayer::Sectors::const_iterator iterS = tSectors.begin(); iterS != tSectors.end() ; ++iterS){

      DeSTSector* aSector = *iterS;

      const DeSTSensor* sensor = findSensor(aSector,aLine3D);
      if ( sensor )
      {

        Gaudi::XYZPoint globalEntry = intersection(aLine3D,sensor->entryPlane());
        Gaudi::XYZPoint globalExit = intersection(aLine3D,sensor->exitPlane());

        Gaudi::XYZPoint localEntry = sensor->toLocal(globalEntry);
        Gaudi::XYZPoint localExit = sensor->toLocal(globalExit);

        unsigned int firstStrip = sensor->localUToStrip(localEntry.x());
        unsigned int lastStrip =  sensor->localUToStrip(localExit.x());

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " -> globalEntry = " << globalEntry << endreq;
          verbose() << " -> globalExit  = " << globalExit << endreq;
          verbose() << " -> localEntry  = " << localEntry << endreq;
          verbose() << " -> localExit   = " << localExit << endreq;
          verbose() << " -> firstStrip = " << firstStrip << " lastStrip = " << lastStrip << endreq;
        }

        // might have to swap...
        if (firstStrip > lastStrip) std::swap(firstStrip, lastStrip);

        // allow for capacitive coupling....
        if (sensor->isStrip(firstStrip-1) == true) --firstStrip;
        if (sensor->isStrip(lastStrip+1) == true) ++lastStrip;

        LHCb::STChannelID elemChan = (*iterS)->elementID();

        for (unsigned int iStrip = firstStrip; iStrip <= lastStrip; ++iStrip)
        {

          LHCb::STChannelID aChan = LHCb::STChannelID(elemChan.type(),
                                                      elemChan.station(),
                                                      elemChan.layer(),
                                                      elemChan.detRegion(),
                                                      elemChan.sector(), iStrip);


          hits.push_back(std::make_pair(aChan,double(iCount)));

        }  // iStrip
        ++iCount;
      } // if
    } // sectors
  }
  else {
    warning() << "failed to find layer " << endmsg;
  }

  return StatusCode::SUCCESS;

};

bool ITExpectedHits::insideLayer(const DeSTLayer* layer,
                                 const Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line, layer->plane() ,point, mu) == true){
    isIn = layer->isInside(point);
  }
  return isIn;
}

DeSTSensor* ITExpectedHits::findSensor(const DeSTSector* sector,
                                       const Line3D& line) const{

  DeSTSensor* sensor = 0;
  const DeSTSector::Sensors& tsensors = sector->sensors();
  DeSTSector::Sensors::const_iterator iter = tsensors.begin();
  for (; iter != tsensors.end() && sensor == 0; ++iter){
    Gaudi::XYZPoint point;
    double mu;
    if (Gaudi::Math::intersection(line, (*iter)->plane() ,point, mu) == true){
      if ((*iter)->globalInActive(point) == true) sensor = *iter;    
    }
  } // loop sensors
  return sensor;
}

Gaudi::XYZPoint ITExpectedHits::intersection(const Line3D& line,
                                             const Gaudi::Plane3D& aPlane) const{

  // make a plane
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}

