
//-----------------------------------------------------------------------------
/** @file RichRayTraceCherenkovCone.cpp
 *
 *  Implementation file for tool : RichRayTraceCherenkovCone
 *
 *  CVS Log :-
 *  $Id: RichRayTraceCherenkovCone.cpp,v 1.14 2006-08-09 11:12:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRayTraceCherenkovCone.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRayTraceCherenkovCone>          s_factory ;
const        IToolFactory& RichRayTraceCherenkovConeFactory = s_factory ;

// Standard constructor
RichRayTraceCherenkovCone::RichRayTraceCherenkovCone( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_rayTrace      ( 0 ),
    m_ckAngle       ( 0 ),
    m_smartIDTool   ( 0 )
{
  // Define interface for this tool
  declareInterface<IRichRayTraceCherenkovCone>(this);
}

StatusCode RichRayTraceCherenkovCone::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",     m_rayTrace );
  acquireTool( "RichCherenkovAngle", m_ckAngle  );
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );

  return sc;
}

StatusCode RichRayTraceCherenkovCone::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id,
                                      std::vector<Gaudi::XYZPoint> & points,
                                      const unsigned int nPoints,
                                      const RichTraceMode mode ) const
{
  return rayTrace( segment,
                   m_ckAngle->avgCherenkovTheta(segment, id),
                   points, nPoints, mode );
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( RichRecSegment * segment,
                                      const double ckTheta,
                                      std::vector<Gaudi::XYZPoint> & points,
                                      const unsigned int nPoints,
                                      const RichTraceMode mode ) const
{
  // make sure segment is valid
  if ( !segment ) Exception( "Null RichRecSegment pointer!" );

  // Do the ray-tracing
  return rayTrace ( segment->trackSegment().rich(),
                    segment->trackSegment().bestPoint(),
                    segment->trackSegment().bestMomentum(),
                    ckTheta, points, nPoints, mode );
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( LHCb::RichRecRing * ring,
                                      const unsigned int nPoints,
                                      const LHCb::RichTraceMode mode,
                                      const bool forceTracing ) const
{
  if ( !ring ) return Error( "Null RichRecRing pointer!" );
  return rayTrace( ring->richRecSegment()->trackSegment().rich(),
                   ring->richRecSegment()->trackSegment().bestPoint(),
                   ring->richRecSegment()->trackSegment().bestMomentum(),
                   ring->radius(),
                   ring, nPoints, mode, forceTracing );
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( const Rich::DetectorType rich,
                                      const Gaudi::XYZPoint & emissionPoint,
                                      const Gaudi::XYZVector & direction,
                                      const double ckTheta,
                                      RichRecRing * ring,
                                      const unsigned int nPoints,
                                      const RichTraceMode mode,
                                      const bool forceTracing ) const
{
  if ( !ring ) return Error( "Null RichRecRing pointer!" );

  if ( !forceTracing && !(ring->ringPoints().empty()) ) return StatusCode::SUCCESS;
  
  ring->ringPoints().clear();

  if ( ckTheta > 0 )
  {

    // Define rotation matrix
    const Gaudi::XYZVector z = direction.unit();
    Gaudi::XYZVector y = z.Cross( Gaudi::XYZVector(0.,1.,0.) );
    y /= sqrt(y.Mag2());
    const Gaudi::XYZVector x = y.Cross(z);
    Gaudi::Rotation3D rotation = Gaudi::Rotation3D(x,y,z);

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Ray tracing " << rich << " CK cone :-" << endreq;
      debug() << " -> emissionPoint = " << emissionPoint << endreq;
      debug() << " -> direction     = " << direction << endreq;
      debug() << " -> CK theta      = " << ckTheta << endreq;
      debug() << " -> " << mode << endreq;
    }

    // Set up cached parameters for photon tracing
    const double incPhi = twopi / static_cast<double>(nPoints);

    // loop around the ring
    const double sinCkTheta = sin(ckTheta);
    const double cosCkTheta = cos(ckTheta);
    double ckPhi = 0.0;
    for ( unsigned int iPhot = 0; iPhot < nPoints; ++iPhot, ckPhi+=incPhi )
    {

      // Photon direction around loop
      const Gaudi::XYZVector photDir =
        rotation * Gaudi::XYZVector( sinCkTheta*cos(ckPhi),
                                     sinCkTheta*sin(ckPhi),
                                     cosCkTheta );

      // temp photon
      RichGeomPhoton photon;

      //if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDTubes )

      RichRecPointOnRing * point = NULL;

      // first ray trace to infinite plane to get point
      RichTraceMode tmpMode( LHCb::RichTraceMode::IgnoreHPDAcceptance );
      if ( m_rayTrace->traceToDetector( rich, emissionPoint, photDir, photon, tmpMode ) )
      {
        ring->ringPoints().push_back( RichRecPointOnRing() );
        point = &(ring->ringPoints().back());
        point->setAcceptance( RichRecPointOnRing::UndefinedAcceptance );
      }

      // next, if configured to do so test acceptance of this point
      if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDPanel ||
           mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDTubes )
      {
        // try again to see if in HPD panel
        tmpMode.setDetPlaneBound( LHCb::RichTraceMode::RespectHPDPanel );
        tmpMode.setDetPrecision( mode.detPrecision() );
        if ( !m_rayTrace->traceToDetector( rich, emissionPoint, photDir, photon, tmpMode ) )
        {
          point->setAcceptance( RichRecPointOnRing::OutsideHPDPanel );
        }
        else
        {  
          point->setAcceptance( RichRecPointOnRing::InHPDPanel );
          if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDTubes )
          {
            // try yet again to see if in an HPD tube
            if ( m_rayTrace->traceToDetector( rich, emissionPoint, photDir, photon, mode ) )
            {
              point->setAcceptance( RichRecPointOnRing::InHPDTube );
            }
          }
        }        
      } // final tests

      // if point found, set final data
      if ( point )
      {
        const Gaudi::XYZPoint & gP = photon.detectionPoint();
        point->setGlobalPosition(gP);
        point->setLocalPosition(m_smartIDTool->globalToPDPanel(gP));
        point->setSmartID( photon.smartID() );
      }

    }

    return StatusCode::SUCCESS;

  }
  else
  {
    // cannot perform ray-tracing
    return StatusCode::FAILURE;
  }

}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( const Rich::DetectorType rich,
                                      const Gaudi::XYZPoint & emissionPoint,
                                      const Gaudi::XYZVector & direction,
                                      const double ckTheta,
                                      std::vector<Gaudi::XYZPoint> & points,
                                      const unsigned int nPoints,
                                      const RichTraceMode mode ) const
{

  if ( ckTheta > 0 )
  {

    // Define rotation matrix
    const Gaudi::XYZVector z = direction.unit();
    Gaudi::XYZVector y = z.Cross( Gaudi::XYZVector(0.,1.,0.) );
    y /= sqrt(y.Mag2());
    const Gaudi::XYZVector x = y.Cross(z);
    Gaudi::Rotation3D rotation = Gaudi::Rotation3D(x,y,z);

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Ray tracing " << rich << " CK cone :-" << endreq;
      debug() << " -> emissionPoint = " << emissionPoint << endreq;
      debug() << " -> direction     = " << direction << endreq;
      debug() << " -> CK theta      = " << ckTheta << endreq;
      debug() << " -> " << mode << endreq;
    }

    // Set up cached parameters for photon tracing
    const double incPhi = twopi / static_cast<double>(nPoints);

    // loop around the ring
    const double sinCkTheta = sin(ckTheta);
    const double cosCkTheta = cos(ckTheta);
    double ckPhi = 0.0;
    for ( unsigned int iPhot = 0; iPhot < nPoints; ++iPhot, ckPhi+=incPhi )
    {

      // Photon direction around loop
      const Gaudi::XYZVector photDir =
        rotation * Gaudi::XYZVector( sinCkTheta*cos(ckPhi),
                                     sinCkTheta*sin(ckPhi),
                                     cosCkTheta );

      // Ray trace to detector plane
      Gaudi::XYZPoint hitPoint;
      if ( m_rayTrace->traceToDetector( rich,
                                        emissionPoint,
                                        photDir,
                                        hitPoint,
                                        mode ) )
      {
        points.push_back( hitPoint );
      }

    }

    return StatusCode::SUCCESS;

  }
  else
  {
    // cannot perform ray-tracing
    return StatusCode::FAILURE;
  }

}
