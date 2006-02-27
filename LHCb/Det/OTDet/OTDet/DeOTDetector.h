// $Id: DeOTDetector.h,v 1.23 2006-02-27 19:33:38 jvantilb Exp $
#ifndef OTDET_DEOTDETECTOR_H
#define OTDET_DEOTDETECTOR_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// OTDet
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

// Kernel
#include "Kernel/OTChannelID.h"
#include "Kernel/LHCbID.h"

// MathCore
#include "Kernel/Point3DTypes.h"

/** @class DeOTDetector DeOTDetector.h "OTDet/DeOTDetector.h"
 *
 *  This is the detector element class for the Outer Tracker. It 
 *  is able to get the geometry from the XML. Many geometry parameters are
 *  stored inside it grand-children, which are derived from DetectorElement
 *  as well: DeOTModule. DeOTDetector is used by the OT digitization and 
 *  the track reconstruction.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   26-05-2002
 */

namespace LHCb{ class Trajectory; }

namespace DeOTDetectorLocation{
  static const std::string& Default="/dd/Structure/LHCb/AfterMagnetRegion/T/OT";
};

static const CLID& CLID_DeOTDetector = 8101;

class DeOTDetector : public DetectorElement {

public:
  
  /// Constructor
  DeOTDetector ( const std::string& name    = "" ) ;
  
  /// Destructor
  ~DeOTDetector () ;
  
  /// object identification
  const CLID& clID () const ;
  /// object identification
  static const CLID& classID () { return CLID_DeOTDetector ; }
  
  /// initialization method 
  virtual StatusCode initialize();

  /// Find the channels and the distances from the MCHits
  StatusCode calculateHits(const Gaudi::XYZPoint& entryPoint, 
                           const Gaudi::XYZPoint& exitPoint,
                           std::vector<LHCb::OTChannelID>& channels,
                           std::vector<double>& driftDistances );

  /// return the station for a given stationID
  DeOTStation* station(unsigned int stationID) const;
  
  /// return the layer for a given channelID
  DeOTLayer* layer(LHCb::OTChannelID aChannel) const;

  /// return the quarter for a given channelID
  // DeOTQuarter* quarter(LHCb::OTChannelID aChannel) const;

  /// return the module for a given channel ID
  DeOTModule* module(LHCb::OTChannelID aChannel) const;

  /// return the module for a given point
  DeOTModule* module(const Gaudi::XYZPoint& point) const;

  /// Return sensor key /sentive volume id for a given global point
  const int sensitiveVolumeID( const Gaudi::XYZPoint& globalPos ) const;
  
  /// return the distance along the wire given a channel and position x,y
  double distanceAlongWire(LHCb::OTChannelID channelID,
                           double xHit,
                           double yHit) const;

  /// return the channel right from a given channel
  LHCb::OTChannelID nextChannelRight(LHCb::OTChannelID aChannel) const;

  /// return the channel left from a given channel
  LHCb::OTChannelID nextChannelLeft(LHCb::OTChannelID aChannel) const;

  /// get the straw resolution
  double resolution() const {return m_resolution;}
  
  /// get the resolution with magn. field correction
  double resolution(const double by) const;

  /// returns the propagation delay (ns/mm)
  double propagationDelay() const {return m_propagationDelay;} ;

  /// Calculate the propagation delay along the wire
  double propagationTime(const LHCb::OTChannelID aChannel, 
                         const double x, const double y ) const;

  /// Calculate max drift as function of By
  double maxDriftTimeFunc(const double by) const;
  
  /// r-t relation without correction for the magnetic field
  double driftTime(const double driftDist) const;
  
  /// r-t relation with correction for the magnetic field
  double driftTime(const double driftDist, const double by) const;

  /// returns the drift delay without magnetic field correction (ns/mm)
  double driftDelay() const {return m_maxDriftTime/m_cellRadius;}

  /// inverse r-t relation without correction for the magnetic field
  double driftDistance( const double driftTime) const;

  /// inverse r-t relation with correction for the magnetic field
  double driftDistance( const double driftTime, const double by ) const;

  /// returns the dead time in ns
  double deadTime() const {return m_deadTime;}
  
  /// get the number of tracker stations
  unsigned int numStations()  { return m_numStations; }

  /// get the first station with OT technology
  unsigned int firstOTStation()  { return m_firstOTStation; }

  /// get the maximum # channels in one module
  unsigned int nMaxChanInModule() { return m_nMaxChanInModule; }

  /// get the vector of all OT modules
  std::vector<DeOTModule*>& modules() { return m_modules; }

  /// get the vector of all OT modules
  const std::vector<DeOTModule*>& modules() const { return m_modules; }

  /// get the total number of readout channels in the OT
  unsigned int nChannels()  { return m_nChannels; }

  /// Returns a Trajectory representing the wire identified by the LHCbID
  /// The offset is zero for all OT Trajectories
  LHCb::Trajectory* trajectory( const LHCb::LHCbID& id,
                                const double = 0 /*offset*/ ) const;

private:

  double m_resolution;            ///< straw resolution
  double m_propagationDelay;      ///< speed of propagation along wire
  double m_maxDriftTime;          ///< maximum drift time
  double m_maxDriftTimeCor;       ///< magn. correction on maximum drift time
  double m_deadTime;              ///< deadtime
  double m_cellRadius;            ///< cell radius

  unsigned int m_numStations;     ///< number of stations
  unsigned int m_firstOTStation;  ///< first OT station
  std::vector<DeOTStation*> m_stations;///< vector of stations
  std::vector<DeOTLayer*> m_layers;    ///< vector of layers
  std::vector<DeOTQuarter*> m_quarters;///< vector of layers
  std::vector<DeOTModule*> m_modules; ///< vector of modules containing geometry
  unsigned int m_nChannels;       ///< total number of channels in OT
  unsigned int m_nMaxChanInModule;///< the maximum # channels in 1 module

  /// Bitmasks for bitfield sensorkey id
  enum sensitiveMasks {moduleMask     = 0x03C00000,
		       quarterMask    = 0x0C000000,
		       layerMask      = 0x30000000,
		       stationMask    = 0xC0000000};
  
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline double DeOTDetector::driftTime(const double driftDist) const
{
  // r-t relation without correction for the magnetic field
  // convert r to t - hack as drift dist can > rCell
  if ( fabs(driftDist) < m_cellRadius ) {
    return ( fabs(driftDist) / m_cellRadius) * m_maxDriftTime;
  } else {
    return m_maxDriftTime;
  }
}

inline double DeOTDetector::driftDistance( const double driftTime) const
{
  // inverse r-t relation without correction for the magnetic field
  return driftTime * m_cellRadius / m_maxDriftTime;
}

inline double DeOTDetector::resolution(const double by) const
{  
  // Calculate resolution
  // The form is p1+p2*B^2 (empirical fit to the testbeam results)
  // p1, p2 vary for different gas mixes
  // The parameterization is only valid for Bx<1.4T (or there abouts)

  return m_resolution ;
}

inline double DeOTDetector::maxDriftTimeFunc(const double by) const
{
  // Calculate max drift as function of B 
  // The form is p1+p2*B^2 (empirical fit to the testbeam results)
  // p1, p2 vary for different gas mixes
  // The parameterization is only valid for Bx<1.4T (or there abouts)

  return (m_maxDriftTime + (m_maxDriftTimeCor * by * by) );
}


#endif  // OTDET_DEOTDETECTOR_H
