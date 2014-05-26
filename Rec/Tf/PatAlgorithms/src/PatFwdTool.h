// $Id: PatFwdTool.h,v 1.7 2009-11-26 18:00:48 mschille Exp $
#ifndef FWDGEOMETRYTOOL_H
#define FWDGEOMETRYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatFwdTrackCandidate.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"

#include "PatKernel/PatForwardHit.h"
#include "Kernel/ILHCbMagnetSvc.h"


static const InterfaceID IID_PatFwdTool ( "PatFwdTool", 1, 0 );
class PatFwdFitLine;
class PatFwdFitParabola;

  /** @class PatFwdTool PatFwdTool.h
   *  This tool holds the parameters to extrapolate tracks
   *  in the field.
   *
   *  @author Olivier Callot
   *  @date   2005-04-01 Initial version
   *  @date   2007-08-20 Update for A-Team tracking framework
   */

class PatFwdTool : public GaudiTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatFwdTool; }

  /// Standard constructor
  PatFwdTool( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  virtual ~PatFwdTool( ); ///< Destructor

  virtual StatusCode initialize();

  double xAtReferencePlane( PatFwdTrackCandidate& track, PatFwdHit* hit, bool store=false );

  double zReference() const { return m_zReference; }

  const std::vector<double>& zOutputs() const { return m_zOutputs; }

  bool fitXCandidate( PatFwdTrackCandidate& track,
  double maxChi2, int minPlanes );

  bool fitStereoCandidate( PatFwdTrackCandidate& track,
  double maxChi2, int minPlanes ) const;
private:
  template <typename Curve>
  bool fitXProjection_ ( PatFwdTrackCandidate& track,
                        PatFwdHits::const_iterator itBeg,
                        PatFwdHits::const_iterator itEnd,
                        bool onlyXPlanes  ) const;
public:
  bool fitXProjection ( PatFwdTrackCandidate& track,
                        PatFwdHits::const_iterator itBeg,
                        PatFwdHits::const_iterator itEnd,
                        bool onlyXPlanes  ) const {
  return m_withoutBField ? fitXProjection_<PatFwdFitLine>( track, itBeg, itEnd, onlyXPlanes )
                         : fitXProjection_<PatFwdFitParabola>( track, itBeg, itEnd, onlyXPlanes );
  }

  void setRlDefault ( PatFwdTrackCandidate& track,
                      PatFwdHits::const_iterator itBeg,
                      PatFwdHits::const_iterator itEnd ) const;

  template <typename Iter>
  void updateHitsForTrack ( const PatFwdTrackCandidate& track, Iter&& begin, Iter&& end ) const {
      auto z0=m_zReference; 
      auto y0=track.y(-z0);
      std::for_each( std::forward<Iter>(begin), std::forward<Iter>(end) , [y0,z0,&track](PatForwardHit *hit) {
        updateHitForTrack( hit, y0, track.ySlope( hit->z()-z0 ) );
      } );
  }

  double distanceForFit( const PatFwdTrackCandidate& track, const PatFwdHit* hit) const {
    double dist =  distanceHitToTrack( track, hit );
    if ( hit->hit()->region() > 1)  return dist;
    return dist / track.cosAfter();
  }

  double distanceHitToTrack( const PatFwdTrackCandidate& track, const PatFwdHit* hit) const {
    double dz   = hit->z() - m_zReference;
    double dist = hit->x() - track.x( dz );
    if ( hit->hit()->region() > 1 ) return dist;

    //OT : distance to a circle, drift time
    dist *= track.cosAfter();
    double dx = hit->driftDistance();
    if ( hit->rlAmb() > 0 ) return dist + dx;
    if ( hit->rlAmb() < 0 ) return dist - dx;

    // Take closest distance in the rest
    return ( fabs( dist - dx ) < fabs( dist + dx ) ) ? (dist - dx) : (dist + dx) ;
  }

  double chi2Magnet( const PatFwdTrackCandidate& track) const {
      //== Error component due to the contraint of the magnet centre
      double dist      = distAtMagnetCenter( track );
      double errCenter = m_xMagnetTol + track.dSlope() * track.dSlope() * m_xMagnetTolSlope;
      return  dist * dist / errCenter;
  }

  double chi2Hit( const PatFwdTrackCandidate& track, const PatFwdHit* hit) const {
    double dist = distanceHitToTrack( track, hit );
    return dist * dist * hit->hit()->weight();
  }

  double distAtMagnetCenter( const PatFwdTrackCandidate& track ) const {
    double dz   = m_zMagnet - m_zReference;
    double dist = track.xStraight( m_zMagnet ) - track.xMagnet( dz );
    return dist;
  }

  double chi2PerDoF( PatFwdTrackCandidate& track ) const;

  bool removeYIncompatible(PatFwdTrackCandidate& track, double tol, int minPlanes ) const {
    bool hasChanged = false;
    for ( PatFwdHit* hit : track.coords() ) {
      if ( !hit->isSelected() ) continue;
      if ( !hit->hit()->isYCompatible( track.y( hit->z() - m_zReference ), tol ) ) {
        hit->setSelected( false );
        hasChanged = true;
      }
    }
    if ( hasChanged ) return fitStereoCandidate( track, 1000000., minPlanes );

    PatFwdPlaneCounter planeCount( track.coordBegin(), track.coordEnd() );
    if ( minPlanes > planeCount.nbDifferent() ) return false;
    return true;
  }

  bool filterOT(const   PatFwdTrackCandidate& track, int minCoord ) const {
    PatFwdRegionCounter regions( track.coordBegin(), track.coordEnd() );
    int nbIT =  regions.nbInRegion( 2 ) +  regions.nbInRegion( 3 ) +
      regions.nbInRegion( 4 ) +  regions.nbInRegion( 5 ) ;
    if ( 1 < nbIT ) return false;
    if ( regions.nbInRegion( 0 ) + regions.nbInRegion( 1 ) < minCoord ) return true;
    return false;
  }

  double changeInY(const  PatFwdTrackCandidate& track ) const {
    double yOriginal = track.yStraight( m_zReference );
    if (!m_withoutBField)
      yOriginal += track.dSlope() * track.dSlope() * track.slY() * m_yParams[0];
    return yOriginal - track.y( 0. );
  }

  double qOverP( const PatFwdTrackCandidate& track ) const;
  double zMagnet( const PatFwdTrackCandidate& track ) const;
  double magscalefactor() const { return m_magFieldSvc->signedRelativeCurrent();} ;

private:
  ILHCbMagnetSvc*     m_magFieldSvc; 
  std::vector<double> m_zMagnetParams;
  std::vector<double> m_xParams;
  std::vector<double> m_yParams;
  std::vector<double> m_momentumParams;
  double m_zReference;
  std::vector<double> m_zOutputs;
  double m_xMagnetTol;
  double m_xMagnetTolSlope;
  double m_zMagnet;

  bool m_withoutBField;

};

#endif // FWDGEOMETRYTOOL_H
