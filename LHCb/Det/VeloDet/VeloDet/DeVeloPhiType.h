// $Id: DeVeloPhiType.h,v 1.4 2004-02-17 21:36:50 mtobin Exp $
#ifndef VELODET_DEVELOPHITYPE_H 
#define VELODET_DEVELOPHITYPE_H 1

// Include files

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

/// from VeloDet
#include "VeloDet/DeVeloSensor.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"


// Unique class identifier
static const CLID& CLID_DeVeloPhiType = 1008103 ;


/** @class DeVeloPhiType DeVeloPhiType.h VeloDet/DeVeloPhiType.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2003-01-14
 */
class DeVeloPhiType : public DeVeloSensor {
public:
  /// Standard constructor
  DeVeloPhiType(  const std::string& name  = ""); 

  ~DeVeloPhiType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloPhiType; };
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  StatusCode pointToChannel(const HepPoint3D& point,
                            VeloChannelID& channel,
                            double& localOffset,
                            double& pitch);
  
  /// Get the nth nearest neighbour for a given channel
  StatusCode neighbour(const VeloChannelID& start, 
                       const int& nOffset, 
                       VeloChannelID& channel);

  /// Returns the number of channels between two channels
  StatusCode channelDistance(const VeloChannelID& start,
                             const VeloChannelID& end,
                             int& nOffset);
  
  /// Residual of 3-d point to a VeloChannelID
  StatusCode residual(const HepPoint3D& point, 
                      const VeloChannelID& channel,
                      double &residual,
                      double &chi2);



  /// Residual [see DeVelo for explanation]
  StatusCode residual(const HepPoint3D& point,
                      const VeloChannelID& channel,
                      const double localOffset,
                      const double width,
                      double &residual,
                      double &chi2);

  /// The capacitance of the strip 
  double stripCapacitance(unsigned int strip);
  
  /// The number of zones in the detector
  inline unsigned int numberOfZones(){return m_numberOfZones;}
  
  /// The zones number for a given strip
  inline unsigned int zoneOfStrip(const unsigned int strip){
    unsigned int zone=0;
    if(m_nbInner > strip) {
      zone = 0;
    } else {
      zone = 1;
    }
    return zone;
  }
  
  /// The number of strips in a zone
  inline unsigned int stripsInZone(const unsigned int zone){
    if(0 == zone) {
      return m_nbInner;
    } else if(1 == zone){
      return m_numberOfStrips-m_nbInner;
    }
    return 0;
  }
  
  /// The minimum radius for a given zone of the sensor
  double rMin(const unsigned int zone);
  
  /// The maximum radius for a given zone of the sensor
  double rMax(const unsigned int zone);

  /// The minimum phi for a given zone of the sensor
  double phiMin(const unsigned int /*zone*/) {return 0.;}
  
  /// The maximum phi for a given zone of the sensor
  double phiMax(const unsigned int /*zone*/) {return 0.;}

  /// Determines if 3-d point is inside sensor
  StatusCode isInside(const HepPoint3D& point);

  /// Determine if point is in corner cut-offs
  bool isCutOff(double x, double y);

  /// The phi position of a strip at a given radius
  inline double phiOfStrip(unsigned int strip, double fraction, 
                           const double radius){
    double effectiveStrip=fraction+static_cast<double>(strip);
    if (m_nbInner > strip) {
      return (effectiveStrip*m_innerPitch) + phiOffset(radius);
    } else {
      effectiveStrip -= m_nbInner;
      return (effectiveStrip*m_outerPitch) + phiOffset(radius);
    }
  }
  
  /// The angle of the strip wrt to the x axis
  inline double angleOfStrip(unsigned int strip, double fraction){
    double effectiveStrip=fraction+static_cast<double>(strip);
    if (m_nbInner > strip) {
      return (effectiveStrip*m_innerPitch) + m_innerTilt;
    } else {
      effectiveStrip -= m_nbInner;
      return (effectiveStrip*m_outerPitch) + m_outerTilt;
    }
  }
  
  /// Returns the offset in phi for a given radius
  inline double phiOffset(double radius){
    if(m_middleRadius > radius){
      return m_innerTilt - asin(m_innerDistToOrigin / radius);
    } else {
      return m_outerTilt - asin(m_outerDistToOrigin / radius);
    }
  }

  /// The phi pitch in mm at a given radius
  inline double phiPitch(const double radius){
    if (m_middleRadius > radius) {
      return m_innerPitch * radius;
    } else {
      return m_outerPitch * radius;
    }
  }

  /// The phi pitch in radians for a given strip
  inline double phiPitch(unsigned int strip){
    if (m_nbInner > strip) {
      return m_innerPitch;
    } else {
      return m_outerPitch;
    }
  }

  /// Return the distance to the origin 
  inline double distToOrigin(unsigned int strip){
    if(m_nbInner > strip){
      return m_innerDistToOrigin;
    } else {
      return m_outerDistToOrigin;
    }
  }

  /// The minimum phi of the sensor for a given radius
  double phiMin(const double radius);
  
  /// The maximum phi of the sensor for a given radius
  double phiMax(const double radius);
  
  /// Return the strip geometry for panoramix
  inline StatusCode stripLimits(unsigned int strip, HepPoint3D& begin,
                                HepPoint3D& end){
    StatusCode sc=this->localToGlobal(m_stripLimits[strip].first,begin);
    if(!sc) return sc;
    sc=this->localToGlobal(m_stripLimits[strip].second,end);
    return sc;
  }
 
protected:

private:
  /// Calculate the equation of the line for each strip
  void calcStripLines();
  /// Calculate the equation of line for the corner cut-offs
  void cornerLimits();
  unsigned int m_numberOfZones;
  unsigned int m_numberOfStrips;
  unsigned int m_nbInner;
  bool m_isDownstream;
  double m_innerRadius;
  double m_outerRadius;
  double m_middleRadius;
  double m_innerTilt;
  double m_innerDistToOrigin;
  double m_outerTilt;
  double m_outerDistToOrigin;
  double m_phiOrigin;
  double m_innerPitch;
  double m_outerPitch;
  double m_rGap;
  std::vector<int> m_stripsInZone;
  std::vector<std::pair<double,double> > m_stripLines;
  /// First corner
  double m_corner1X1;
  double m_corner1Y1;
  double m_corner1X2;
  double m_corner1Y2;
  /// Second corner 
  double m_corner2X1;
  double m_corner2Y1;
  double m_corner2X2;
  double m_corner2Y2;

  std::pair<double,double> m_resolution;
  /// Define line of cutoffs (first is gradient, second is intercept)
  std::vector<std::pair<double,double> > m_cutOffs;
  //  std::vector<double> m_gradientCutOff;
  //std::vector<double> m_interceptCutOff;
  std::vector<std::pair<HepPoint3D,HepPoint3D> > m_stripLimits;
  double m_innerCoverage;
  double m_outerCoverage;
  double m_halfCoverage;
  bool m_down;
  
};
#endif // VELODET_DEVELOPHITYPE_H
