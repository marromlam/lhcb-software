// $Id: DeOTLayer.h,v 1.6 2006-03-30 21:45:32 janos Exp $
#ifndef OTDET_DEOTLAYER_H
#define OTDET_DEOTLAYER_H 1

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"

/// OTDet
#include "OTDet/DeOTDetector.h"

/** @class DeOTLayer DeOTLayer.h "OTDet/DeOTLayer.h"
 *
 *  This is the detector element class for an Outer Tracker Layer.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

/// Forward declarations
class DeOTQuarter;
class DeOTModule;

namespace LHCb
{
  class Point3DTypes;
}

static const CLID& CLID_DeOTLayer = 8103;

class DeOTLayer : public DetectorElement {

 public:

  /** Some typedefs */
  typedef std::vector<DeOTQuarter*> Quarters;

  /** Constructor */
  DeOTLayer(const std::string& name = "") ;
  
  /** Destructor */
  ~DeOTLayer() ;
  
  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const ;

  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTLayer; }
  
  /** Initialization method 
   * @return Status of initialisation
   */
  virtual StatusCode initialize();

  /** @return layerID */
  unsigned int layerID() const;
  
  /** Element id */
  LHCb::OTChannelID elementID() const;
  
  /** Set element id */
  void setElementID(const LHCb::OTChannelID& chanID);

  /** check contains channel
   *  @param channel
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;
    
  /** @return stereo angle of the layer */
  double angle() const;

  /** @return the stereo angle of the layer */
  double stereoAngle() const;
  
  /** @return the quarter for a given channelID */
  DeOTQuarter* findQuarter(const LHCb::OTChannelID aChannel) const;
  
  /** @return the quarter for a given XYZ point */
  /** This method was depracated but I need it for the sensitive volume ID */
  DeOTQuarter* findQuarter(const Gaudi::XYZPoint& aPoint) const;

  /** @return the module for a given XYZ point */
  DeOTModule* findModule(const Gaudi::XYZPoint& aPoint) const;

  /** flat vector of quarters
   * @return vector of quarters
   */
  const Quarters& quarters() const;
  
 private:
  unsigned int m_stationID;      ///< stationID
  unsigned int m_layerID;        ///< layer ID number
  LHCb::OTChannelID m_elementID; ///< element id
  double m_stereoAngle;          ///< layer stereo angle 
  Quarters m_quarters;           ///< vector of quarters

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeOTLayer::layerID() const {
  return m_layerID;
}

inline LHCb::OTChannelID DeOTLayer::elementID() const {
  return m_elementID;
}

inline void DeOTLayer::setElementID(const LHCb::OTChannelID& chanID) {
  m_elementID = chanID;
}

inline bool DeOTLayer::contains(const LHCb::OTChannelID aChannel) const {
  return (layerID() == aChannel.layer());
}

inline double DeOTLayer::angle() const {
  return m_stereoAngle;
}

inline double DeOTLayer::stereoAngle() const {
  return m_stereoAngle;
}

inline const DeOTLayer::Quarters& DeOTLayer::quarters() const {
  return m_quarters;
}

#endif  // OTDET_DEOTLAYER_H
