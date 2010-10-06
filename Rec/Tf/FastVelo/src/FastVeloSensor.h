// $Id: $
#ifndef FASTVELOSENSOR_H 
#define FASTVELOSENSOR_H 1

// Include files
#include "FastVeloHit.h"
#include "VeloDet/DeVelo.h"

/** @class FastVeloSensor FastVeloSensor.h
 *  Describes one sensor and its data for the Fast hit manager
 *
 *  @author Olivier Callot
 *  @date   2010-09-08
 */
class FastVeloSensor {
public: 
  /// Standard constructor
  FastVeloSensor( DeVeloRType* rSensor ); 

  FastVeloSensor( DeVeloPhiType* phiSensor ); 

  virtual ~FastVeloSensor( ) {}; ///< Destructor

  unsigned int number()    const { return m_number; }
  int next( bool forward ) const { return forward ? m_previous : m_next;  }
  bool isRight()           const { return m_isRight; }
  bool isRSensor()         const { return m_isRSensor; }
  double z()               const { return m_z; }
  double z( double x, double y ) const { return m_z + x * m_dzDx + y * m_dzDy; }
  double xCentre()         const { return m_centre.x(); }
  double yCentre()         const { return m_centre.y(); }
  double rMin()            const { return m_sensor->innerRadius();}
  double rMax()            const { return m_sensor->outerRadius();}
  double rMin( unsigned int zone ) const { return m_phiSensor->rMin( zone );}
  double rMax( unsigned int zone ) const { return m_phiSensor->rMax( zone );}
  FastVeloHits&  hits  ( unsigned int zone ) { return m_hits[zone]; }  

  void setPrevious( int prev ) { m_previous = prev; }
  void setNext( int next )     { m_next     = next; }

  DeVeloRType* rSensor() { return m_rSensor; }
  DeVeloPhiType* phiSensor() { return m_phiSensor; }

  FastVeloLineParams lineParams(  unsigned int strip, double frac ) const {
    if ( 0. == frac ) return  m_lineParams[strip];
    FastVeloLineParams first = m_lineParams[strip];
    FastVeloLineParams sec   = m_lineParams[strip+1];
    return FastVeloLineParams( (1-frac)*first.a() + frac * sec.a(),
                               (1-frac)*first.b() + frac * sec.b(),
                               (1-frac)*first.c() + frac * sec.c(),
                               (1-frac)*first.xs() + frac * sec.xs(),
                               (1-frac)*first.ys() + frac * sec.ys());
  }
                           
  double rPitch( double r ) const { return m_rSensor->rPitch( r ); }

  double rOffset( unsigned int zone ) const { return m_rOffset[zone]; }

protected:

  void setGeometry ( DeVeloSensor* sensor );

private:
  unsigned int    m_number;
  int             m_next;
  int             m_previous;
  bool            m_isRSensor;
  bool            m_isRight;
  double          m_z;

  DeVeloRType*    m_rSensor;
  DeVeloPhiType*  m_phiSensor;
  DeVeloSensor*   m_sensor;

  Gaudi::XYZPoint m_centre;
  double          m_dzDx;
  double          m_dzDy;

  std::vector<FastVeloHits>   m_hits;
  std::vector<double>         m_rOffset;

  //== Line parameterization for phi
  std::vector<FastVeloLineParams> m_lineParams;
};
#endif // FASTVELOSENSOR_H
