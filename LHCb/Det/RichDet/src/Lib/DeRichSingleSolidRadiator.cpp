
/** @file DeRichSingleSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichSingleSolidRadiator
 *
 *  $Id: DeRichSingleSolidRadiator.cpp,v 1.10 2005-02-23 10:26:00 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSINGLESOLIDRADIATOR_CPP

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// CLHEP files
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

/// Detector description classes
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/SolidTrd.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichSingleSolidRadiator = 12040;  // User defined

// Standard Constructor
DeRichSingleSolidRadiator::DeRichSingleSolidRadiator() {}

// Standard Destructor
DeRichSingleSolidRadiator::~DeRichSingleSolidRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichSingleSolidRadiator::classID() 
{
  return CLID_DeRichSingleSolidRadiator;
}

StatusCode DeRichSingleSolidRadiator::initialize()
{
  if ( DeRichRadiator::initialize().isFailure() ) return StatusCode::FAILURE;

  MsgStream log( msgSvc(), myName() );
  log << MSG::DEBUG << "Starting initialisation for DeRichSingleSolidRadiator "
      << name() << endreq;

  m_solid = geometry()->lvolume()->solid();

  const Material::Tables& myTabProp = geometry()->lvolume()->material()->tabulatedProperties();
  Material::Tables::const_iterator matIter;
  for ( matIter = myTabProp.begin(); matIter!=myTabProp.end(); ++matIter ) {
    if ( (*matIter) ) {
      if ( (*matIter)->type() == "RINDEX" ) {
        m_refIndex = (*matIter);
      }
      if ( (*matIter)->type() == "RAYLEIGH" ) {
        m_rayleigh = (*matIter);
      }
    }
  }

  if (!m_refIndex) {
    log << MSG::ERROR << "Radiator " << name() << " without refractive index"
        << endmsg;
    return StatusCode::FAILURE;
  }

  const HepPoint3D zero(0.0, 0.0, 0.0);
  log << MSG::DEBUG << "Found  TabProp " << m_refIndex->name() << " type "
      << m_refIndex->type() << endmsg;
  if ( m_rayleigh )
    log << MSG::DEBUG << "Found  TabProp " << m_rayleigh->name() << " type "
        << m_rayleigh->type() << endmsg;
  log << MSG::DEBUG <<" Centre:" << geometry()->toGlobal(zero) << endreq;

  return StatusCode::SUCCESS;
}

StatusCode
DeRichSingleSolidRadiator::nextIntersectionPoint( const HepPoint3D&  pGlobal,
                                                  const HepVector3D& vGlobal,
                                                  HepPoint3D&  returnPoint ) const
{

  const HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }
  else {
    returnPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
    return StatusCode::SUCCESS;
  }

}

//=========================================================================
//
//=========================================================================
StatusCode
DeRichSingleSolidRadiator::intersectionPoints( const HepPoint3D&  position,
                                               const HepVector3D& direction,
                                               HepPoint3D& entryPoint,
                                               HepPoint3D& exitPoint ) const
{

  const HepPoint3D pLocal( geometry()->toLocal(position) );
  HepVector3D vLocal( direction );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks)  return StatusCode::FAILURE;

  entryPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
  exitPoint  = geometry()->toGlobal( pLocal + ticks[noTicks-1] * vLocal );
  return StatusCode::SUCCESS;

}


//=========================================================================
//
//=========================================================================
unsigned int
DeRichSingleSolidRadiator::intersectionPoints( const HepPoint3D& pGlobal,
                                               const HepVector3D& vGlobal,
                                               std::vector<HepPoint3D>&
                                               points) const
{

  const HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (noTicks != 0) {
    for (ISolid::Ticks::iterator tick_it = ticks.begin();
         tick_it != ticks.end();
         ++tick_it) {
      points.push_back( HepPoint3D( geometry()->toGlobal( pLocal + (*tick_it) * vLocal) ) );
    }
  }
  return noTicks;
}
