// $Id: DeVeloSensor.h,v 1.5 2004-02-28 21:43:16 mtobin Exp $
#ifndef VELODET_DEVELOSENSOR_H 
#define VELODET_DEVELOSENSOR_H 1

// Include files
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"


// Unique class identifier
static const CLID& CLID_DeVeloSensor = 1008101 ;


/** @class DeVeloSensor DeVeloSensor.h VeloDet/DeVeloSensor.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2003-01-14
 */
class DeVeloSensor : public DetectorElement {
public:
  /// Standard constructor
  DeVeloSensor( const std::string& name = "" ); 

  /// Destructor
  virtual ~DeVeloSensor( ); 

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloSensor; };
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  virtual StatusCode pointToChannel(const HepPoint3D& point,
                                    VeloChannelID& channel,
                                    double& localOffset,
                                    double& pitch)=0;
  
  /// Get the nth nearest neighbour for a given channel
  virtual StatusCode neighbour(const VeloChannelID& start, 
                               const int& nOffset, 
                               VeloChannelID& channel)=0;

  /// Returns the number of channels between two channels
  virtual StatusCode channelDistance(const VeloChannelID& start,
                                     const VeloChannelID& end,
                                     int& nOffset)=0;
  
  /// Residual of 3-d point to a VeloChannelID
  virtual StatusCode residual(const HepPoint3D& point, 
                              const VeloChannelID& channel,
                              double &residual,
                              double &chi2)=0;

  /// Residual [see DeVelo for explanation]
  virtual StatusCode residual(const HepPoint3D& point,
                              const VeloChannelID& channel,
                              const double localOffset,
                              const double width,
                              double &residual,
                              double &chi2)=0;
  
  /// The capacitance of the strip 
  virtual double stripCapacitance(unsigned int strip)=0;
  
  /// The zones number for a given strip
  //  virtual unsigned int zoneOfStrip(const VeloChannelID& channel)=0;
  virtual unsigned int zoneOfStrip(const unsigned int strip)=0;
  
  /// The number of strips in a zone
  virtual unsigned int stripsInZone(const unsigned int zone)=0;

  /// The minimum radius of the sensitive area of a zone
  virtual double rMin(const unsigned int zone)=0;
  
  /// The maximum radius of the sensitive area of a zone
  virtual double rMax(const unsigned int zone)=0;

  /// The minimum phi for a given zone of the sensor
  //  virtual double phiMin(const unsigned int zone)=0;
  
  /// The maximum phi for a given zone of the sensor
  //  virtual double phiMax(const unsigned int zone)=0;
  
  /// Determine if point is in corner cut-offs
  virtual  StatusCode isInside(const HepPoint3D& point)=0;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y)=0;

  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)
  StatusCode localToGlobal(const HepPoint3D& local, 
                                   HepPoint3D& global);

  /// Convert global position to local position 
  StatusCode globalToLocal(const HepPoint3D& global, 
                                   HepPoint3D& local);

  /// Convert local phi to rough global phi
  inline double localPhiToGlobal(double phiLocal){
    if(m_isDownstream) phiLocal = -phiLocal;
    if(m_isRight) phiLocal += pi;
    return phiLocal;
  }

  /// Return the z position of the sensor
  inline double z() {return m_z;}
  
  /// Return +1 for X>0 side of the detector (+ve x is Left/L)
  inline int xSide() {return m_xSide;}
  
  /// Return true for X<0 side of the detector (-ve x is Right)
  inline bool isRight() {return m_isRight;}
  
  /// Returns true if sensor is downstream
  inline bool isDownstream() {return m_isDownstream;}

  /// Returns true if R Sensor
  inline bool isPileUp() {return m_isPileUp;}  

  /// Returns true if R Sensor
  inline bool isR() {return m_isR;}

  /// Returns true if R Sensor
  inline bool isPhi() {return m_isPhi;}

  /// Return the number of strips
  inline unsigned int numberOfStrips() {return m_numberOfStrips;}

  /// The number of zones in the detector
  inline unsigned int numberOfZones() {return m_numberOfZones;}
  
  /// The minimum radius for the sensitive area of the sensor
  inline double innerRadius() {return m_innerRadius;}

  /// The maximum radius for the sensitive area of the sensor
  inline double outerRadius() {return m_outerRadius;}
  
  /// The thickness of the sensor in mm
  inline double siliconThickness() {return m_siliconThickness;}
  
  /// Returns the sensor type
  inline std::string type() {return m_type;}

  /// Set the sensor number
  //void sensorNumber(unsigned int sensor);
  inline void sensorNumber(unsigned int sensor){m_sensorNumber=sensor;}

  /// Returns the sensor number
  inline unsigned int sensorNumber() {return m_sensorNumber;}
  
  /// Set the sensors associated with this sensor
  inline void associateSensor(unsigned int sensor) {
    m_associated.push_back(sensor);
  }

  /// Return the (phi) sensors associated with this (R) sensor
  inline std::vector<unsigned int> associatedSensors() {return m_associated;}
  
protected:
  bool m_isRight,m_isLeft,m_isDownstream;
  bool m_isR,m_isPhi,m_isPileUp;
  unsigned int m_numberOfStrips,m_numberOfZones;
  double m_innerRadius,m_outerRadius;

private:
  void initSensor();
  int m_xSide;
  unsigned int m_sensorNumber;
  std::vector<unsigned int> m_associated;
  double m_siliconThickness;
  std::string m_type;
  double m_z;
  IGeometryInfo* m_geometry;
};
#endif // VELODET_DEVELOSENSOR_H
