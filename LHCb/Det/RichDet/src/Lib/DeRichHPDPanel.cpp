
//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.cpp,v 1.29 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#define DERICHHPDPANEL_CPP

// Include files
#include "GaudiKernel/SmartDataPtr.h"

#include "RichDet/DeRichHPDPanel.h"

// MathCore files
#include "Kernel/SystemOfUnits.h"
#include "Kernel/Transform3DTypes.h"

// DetDesc
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

// GSL
#include "gsl/gsl_math.h"

//----------------------------------------------------------------------------

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel() {}

// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}

//=========================================================================
//  Initialize
//=========================================================================
StatusCode DeRichHPDPanel::initialize()
{

  // store the name of the panel, without the /dd/Structure part
  const std::string::size_type pos = name().find("Rich");
  m_name = ( std::string::npos != pos ? name().substr(pos) : "DeRichHPDPanel_NO_NAME" );

  MsgStream msg ( msgSvc(), "DeRichHPDPanel" );

  // Work out what Rich/panel I am
  m_rich = Rich::InvalidDetector;
  m_side = Rich::InvalidSide;
  if      ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    if      ( name().find("PDPanel0") != std::string::npos )
    {
      m_side = Rich::top;
    }
    else if ( name().find("PDPanel1") != std::string::npos )
    {
      m_side = Rich::bottom;
    }
  }
  else if ( name().find("Rich2") != std::string::npos )
  {
    m_rich = Rich::Rich2;
    if      ( name().find("PDPanel0") != std::string::npos )
    {
      m_side = Rich::left;
    }
    else if ( name().find("PDPanel1") != std::string::npos )
    {
      m_side = Rich::right;
    }
  }
  if ( m_rich == Rich::InvalidDetector ||
       m_side == Rich::InvalidSide )
  {
    msg << MSG::ERROR << "Error initializing HPD panel " << name() << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << "Initializing HPD Panel : " << rich()
      << " Panel" << (int)side() << endreq;

  std::string rich1Location;
  if ( name().find("Magnet") == std::string::npos )
    rich1Location = DeRichLocation::Rich1_old;
  else
    rich1Location = DeRichLocation::Rich1;

  SmartDataPtr<DetectorElement> deRich1(dataSvc(),DeRichLocation::Rich1 );
  if (!deRich1) {
    msg << MSG::ERROR << "Could not load DeRich1" << endmsg;
    return StatusCode::FAILURE;
  }
  m_pixelSize = deRich1->param<double>("RichHpdPixelXsize");
  m_subPixelSize = m_pixelSize/8;
  m_activeRadius = deRich1->param<double>("RichHpdActiveInpRad");
  m_activeRadiusSq = m_activeRadius*m_activeRadius;

  m_pixelColumns = deRich1->param<int>("RichHpdNumPixelCol");
  m_pixelRows = deRich1->param<int>("RichHpdNumPixelRow");
  msg << MSG::DEBUG << "pixelRows:" << m_pixelRows << " pixelColumns:"
      << m_pixelColumns << endreq;

  m_HPDRows = param<int>("PDRows");
  m_HPDColumns = param<int>("PDColumns");
  msg << MSG::DEBUG << "RichHpdPixelsize:" << m_pixelSize << " ActiveRadius:"
      << m_activeRadius << " PDRows:" << m_HPDRows << " PDColumns:"
      << m_HPDColumns << endreq;

  /// the next two variables are used in cdf type panel
  m_HPDsInBigCol = static_cast<int>(ceil(m_HPDRows/2.0));
  m_HPDsIn2Cols = 2*m_HPDsInBigCol - 1;

  SmartDataPtr<TabulatedProperty>
    HPDdeMag(dataSvc(),
             "/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  if (!HPDdeMag) {
    msg << MSG::ERROR << "Could not load HpdDemagnification" << endmsg;
    return StatusCode::FAILURE;
  }
  TabulatedProperty::Table DeMagTable = HPDdeMag->table();
  m_deMagFactor[0] = DeMagTable[0].second;
  m_deMagFactor[1] = DeMagTable[1].second;
  msg << MSG::DEBUG << "DeMagFactor(0):" << m_deMagFactor[0]
      << " DeMagFactor(1):" << m_deMagFactor[1] << endreq;

  const Gaudi::XYZPoint zero(0.0, 0.0, 0.0);

  // get the first HPD and follow down to the silicon block
  const IPVolume* pvHPDMaster0 = geometry()->lvolume()->pvolume(0);
  const IPVolume* pvHPDSMaster0 = pvHPDMaster0->lvolume()->pvolume(0);
  const IPVolume* pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume(4);

  const ISolid* siliconSolid = pvSilicon0->lvolume()->solid();
  msg << MSG::DEBUG << "About to do a dynamic cast SolidBox" << endreq;
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);

  // assume same size for all silicon detectors
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // HPD #0 coordinates
  m_HPD0Centre = pvHPDMaster0->toMother(zero);
  msg << MSG::DEBUG << "Centre of HPDPanel:" << geometry()->toGlobal(zero)
      <<endreq;
  msg << MSG::DEBUG<< "Centre of HPD#0:" << geometry()->toGlobal(m_HPD0Centre)
      <<endreq;

  //get the next HPD. For Rich1 same row, for Rich2 same column
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  m_HPD1Centre = pvHPDMaster1->toMother(zero);

  //get the HPD at next row for Rich1, next column for Rich2, NS == next set
  const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDForNS());
  m_HPDNSCentre = pvHPDMasterNS->toMother(zero);

  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume()->pvolume(2);
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  // get the inside radius of the window
  ISolid::Ticks windowTicks;
  unsigned int windowTicksSize = windowSolid0->
    intersectionTicks(Gaudi::XYZPoint(0.0, 0.0, 0.0),Gaudi::XYZVector(0.0, 0.0, 1.0),
                      windowTicks);
  if (windowTicksSize != 2) {
    msg << MSG::FATAL << "Problem getting window radius" << endreq;
    return StatusCode::FAILURE;
  }
  m_winR = windowTicks[0];
  m_winRsq = m_winR*m_winR;

  // get the coordinate of the centre of the HPD quarz window
  Gaudi::XYZPoint HPDTop1(0.0, 0.0, m_winR);
  // convert this to HPDS master  coordinates
  Gaudi::XYZPoint HPDTop2 = pvWindow0->toMother(HPDTop1);
  // and to silicon
  m_HPDTop = pvSilicon0->toLocal(HPDTop2);

  // find the top of 3 HPDs to create a detection plane.  We already have the
  // first in HPDSMaster coordinates.
  // now to HPD coordinates
  Gaudi::XYZPoint pointAInHPD = pvHPDSMaster0->toMother(HPDTop2);
  Gaudi::XYZPoint pointAInPanel = pvHPDMaster0->toMother(pointAInHPD);
  Gaudi::XYZPoint pointA = geometry()->toGlobal(pointAInPanel);

  // for second point go to HPD at the end of the column.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB = geometry()->lvolume()->pvolume(HPDForB());
  Gaudi::XYZPoint pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  Gaudi::XYZPoint pointB = geometry()->toGlobal(pointBInPanel);

  // now point C at the other end.
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(HPDForC());
  Gaudi::XYZPoint pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  Gaudi::XYZPoint pointC = geometry()->toGlobal(pointCInPanel);

  m_detectionPlane = Gaudi::Plane3D(pointA, pointB, pointC);
  m_localPlane = Gaudi::Plane3D(pointAInPanel, pointBInPanel, pointCInPanel);
  m_localPlaneNormal = m_localPlane.Normal();

  // store the z coordinate of the detection plane
  m_detPlaneZ = pointAInPanel.z();

  // localPlane2 is used when trying to locate the HPD row/column from
  // a point in the panel.
  //m_localPlane2 = m_localPlane;
  m_detPlaneZdiff = m_winR - sqrt( m_winRsq - m_activeRadiusSq );
  //m_localPlane2.transform(HepTranslateZ3D(m_detPlaneZdiff));
  m_localPlane2 = Gaudi::Transform3D(Gaudi::XYZVector(0.0, 0.0, m_detPlaneZdiff))( m_localPlane );
  m_localPlaneNormal2 = m_localPlane2.Normal();

  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();

  // Cache HPD information
  m_pvHPDMaster.clear();
  m_pvHPDSMaster.clear();
  m_pvSilicon.clear();
  m_pvWindow.clear();
  m_HPDCentres.clear();
  for ( unsigned int HPD = 0; HPD < PDMax(); ++HPD ) {
    const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPD);
    if ( !pvHPDMaster )
    {
      msg << MSG::ERROR << "Failed to access HPDMaster" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
    if ( !pvHPDSMaster )
    {
      msg << MSG::ERROR << "Failed to access HPDSMaster" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
    if ( !pvWindow )
    {
      msg << MSG::ERROR << "Failed to access HPDWindow" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);
    if ( !pvSilicon )
    {
      msg << MSG::ERROR << "Failed to access HPDSilicon" << endreq;
      return  StatusCode::FAILURE;
    }
    m_pvHPDMaster.push_back( pvHPDMaster );
    m_pvHPDSMaster.push_back( pvHPDSMaster );
    m_pvSilicon.push_back( pvSilicon );
    m_pvWindow.push_back( pvWindow );
    m_HPDCentres.push_back( pvHPDMaster->toMother(zero) );
    m_trans1.push_back( geometry()->matrixInv() * pvHPDMaster->matrixInv() *
                        pvHPDSMaster->matrixInv() * pvWindow->matrixInv() );
    m_trans2.push_back( pvSilicon->matrix() * pvHPDSMaster->matrix() * pvHPDMaster->matrix() );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// convert a point on the silicon sensor to smartID
//=========================================================================
StatusCode DeRichHPDPanel::smartID ( const Gaudi::XYZPoint& globalPoint,
                                     LHCb::RichSmartID& id ) const
{

  const Gaudi::XYZPoint inPanel = geometry()->toLocal(globalPoint);

  // find the HPD row/col of this point
  if ( !findHPDRowCol(inPanel, id) ) return StatusCode::FAILURE;

  const unsigned int HPDNumber = HPDRowColToNum(id.hpdNumInCol(), id.hpdCol());

  const Gaudi::XYZPoint inSilicon = m_trans2[HPDNumber] * inPanel;

  double inSiliconX = inSilicon.x();
  double inSiliconY = inSilicon.y();

  if ( (fabs(inSiliconX)+0.001*mm) > m_siliconHalfLengthX ) {
    const int signX = ( inSiliconX > 0.0 ? 1 : -1 );
    inSiliconX -= signX*0.001*mm;
  }

  if ( (fabs(inSiliconY)+0.001*mm) > m_siliconHalfLengthY ) {
    const int signY = ( inSiliconY > 0.0 ? 1 : -1 );
    inSiliconY -= signY*0.001*mm;
  }

  if ( (fabs(inSiliconX) - m_siliconHalfLengthX > 1E-3*mm) ||
       (fabs(inSiliconY) - m_siliconHalfLengthY > 1E-3*mm)   ) {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::ERROR << "Point " << inSilicon << " is outside the silicon box "
        << m_pvHPDMaster[HPDNumber]->name() << endreq;
    return StatusCode::FAILURE;
  }

  const unsigned int pixelColumn = static_cast<unsigned int>
    ((m_siliconHalfLengthX + inSiliconX) / m_pixelSize);
  const unsigned int pixelRow    = static_cast<unsigned int>
    ((m_siliconHalfLengthY + inSiliconY) / m_pixelSize);

  id.setPixelRow(pixelRow);
  id.setPixelCol(pixelColumn);

  const unsigned int subPixel = static_cast<unsigned int>
    ((m_siliconHalfLengthX+inSiliconX-pixelColumn*m_pixelSize) /
     m_subPixelSize);
  id.setPixelSubRow( subPixel );

  return StatusCode::SUCCESS;
}



//=========================================================================
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::detectionPoint ( const LHCb::RichSmartID& smartID ) const
{

  const unsigned int HPDNumber = HPDRowColToNum(smartID.hpdNumInCol(),smartID.hpdCol());

  // convert pixel number to silicon coordinates
  const double inSiliconX =
    smartID.pixelCol() * m_pixelSize+m_pixelSize/2.0 - m_siliconHalfLengthX;
  const double inSiliconY =
    smartID.pixelRow() * m_pixelSize+m_pixelSize/2.0 - m_siliconHalfLengthY;

  const double inWindowX = -inSiliconX / m_deMagFactor[0];
  const double inWindowY = -inSiliconY / m_deMagFactor[0];
  const double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);

  return (m_trans1[HPDNumber] * Gaudi::XYZPoint(inWindowX,inWindowY,inWindowZ));
}


//=========================================================================
//  convert a point from the global to the panel coodinate system
//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::globalToPDPanel( const Gaudi::XYZPoint& globalPoint ) const
{
  const Gaudi::XYZPoint localPoint( geometry()->toLocal( globalPoint ) );
  return Gaudi::XYZPoint( localPoint.x(), localPoint.y(), localPoint.z()-m_detPlaneZ );
}

//=========================================================================
//  find an intersection with the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                                          const Gaudi::XYZPoint& pGlobal,
                                          Gaudi::XYZPoint& windowPointGlobal,
                                          LHCb::RichSmartID& smartID,
                                          const RichTraceMode mode ) const
{

  // transform point and vector to the HPDPanel coordsystem.
  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vLocal( m_vectorTransf*vGlobal );

  // find the intersection with the detection plane (localPlane2)
  const double scalar = vLocal.Dot(m_localPlaneNormal2);
  if ( scalar == 0.0 ) return StatusCode::FAILURE;

  const double distance = -m_localPlane2.Distance( pLocal )/scalar;
  const Gaudi::XYZPoint panelIntersection( pLocal + distance*vLocal );

  unsigned int  HPDNumber(0);
  LHCb::RichSmartID id( smartID );

  if ( !findHPDRowCol(panelIntersection, id) ) return StatusCode::FAILURE;

  HPDNumber = HPDRowColToNum(id.hpdNumInCol(), id.hpdCol());

  if ( mode.detPrecision() == RichTraceMode::circle ) {  // do it quickly

    const double x = panelIntersection.x() - m_HPDCentres[HPDNumber].x();
    const double y = panelIntersection.y() - m_HPDCentres[HPDNumber].y();
    if ( ( x*x + y*y ) > m_activeRadiusSq ) return StatusCode::FAILURE;
    
    windowPointGlobal = geometry()->toGlobal( panelIntersection );
    
    smartID = id;
    
    return StatusCode::SUCCESS;
  }

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  Gaudi::XYZPoint pInWindow;
  Gaudi::XYZVector vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;
  unsigned int noTicks(0);

  // Overwise slow

  // find the correct HPD and quartz window inside it
  pvHPDMaster = m_pvHPDMaster[HPDNumber];
  
  // just in case
  if ( !pvHPDMaster ) {
    MsgStream msg(msgSvc(), myName() );
    msg << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
        << " from HPDRow:" << id.hpdNumInCol() << " and HPDColumn:" << id.hpdCol() << endreq
        << " x:" << panelIntersection.x()
        << " y:" << panelIntersection.y() <<  endreq;
    return StatusCode::FAILURE;
  }

  pvHPDSMaster = m_pvHPDSMaster[HPDNumber];
  pvWindow     = m_pvWindow[HPDNumber];
  windowSolid  = pvWindow->lvolume()->solid();

  // convert point to local coordinate systems
  pInWindow = pvWindow->toLocal(pvHPDSMaster->toLocal(pvHPDMaster->toLocal(pLocal)));

  // convert local vector assuming that only the HPD can be rotated
  vInHPDMaster = pvHPDMaster->matrix()*vLocal;

  noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                           HPDWindowTicks );
  if ( 0 == noTicks ) return StatusCode::FAILURE;

  Gaudi::XYZPoint windowPoint( pInWindow + HPDWindowTicks[1]*vInHPDMaster );
  Gaudi::XYZPoint windowPointInHPD( pvHPDSMaster->
                                    toMother(pvWindow->toMother(windowPoint)) );
  // check the active radius.
  const double hitRadius2 = ( windowPointInHPD.x()*windowPointInHPD.x() +
                              windowPointInHPD.y()*windowPointInHPD.y() );
  if ( hitRadius2 > m_activeRadiusSq ) return StatusCode::FAILURE;

  windowPointGlobal =
    geometry()->toGlobal( pvHPDMaster->toMother(windowPointInHPD) );

  smartID = id;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode
DeRichHPDPanel::readoutChannelList ( LHCb::RichSmartID::Vector& readoutChannels ) const
{

  // Square of active radius
  const double activeRadiusSq = gsl_pow_2(m_activeRadius*m_deMagFactor[0]);

  for ( unsigned int PD = 0; PD < PDMax(); ++PD )
  {
    // Get HPD row and column numbers outside loops.
    const unsigned int pdRow = PDRow(PD);
    const unsigned int pdCol = PDCol(PD);

    // Loop over pixels
    for ( unsigned int pixRow = 0; pixRow < m_pixelRows; ++pixRow )
    {
      for ( unsigned int pixCol = 0; pixCol < m_pixelColumns; ++pixCol )
      {

        const double xpix  = (pixRow + 0.5)*m_pixelSize - m_siliconHalfLengthX;
        const double ypix  = (pixCol + 0.5)*m_pixelSize - m_siliconHalfLengthY;
        const double xcorn = ( xpix < 0.0 ? xpix+0.5*m_pixelSize : xpix-0.5*m_pixelSize );
        const double ycorn = ( ypix < 0.0 ? ypix+0.5*m_pixelSize : ypix-0.5*m_pixelSize );
        const double radcornSq = xcorn*xcorn + ycorn*ycorn;
        if ( radcornSq <= activeRadiusSq )
        {
          // Add a smart ID for this pixel to the vector
          readoutChannels.push_back ( LHCb::RichSmartID(rich(),side(),pdRow,pdCol,pixRow,pixCol) );
        }

      } // loop over pixel columns
    } // loop over pixel rows

  } // loop over HPDs

  return StatusCode::SUCCESS;
}


//=========================================================================
//  returns the intersection point with the detection plane
//=========================================================================
bool DeRichHPDPanel::detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                                    const Gaudi::XYZVector& vGlobal,
                                    Gaudi::XYZPoint& hitPosition,
                                    const RichTraceMode mode) const
{

  // transform to the Panel coord system.
  Gaudi::XYZVector vLocal( m_vectorTransf*vGlobal );

  const double scalar = vLocal.Dot(m_localPlaneNormal);
  if ( scalar == 0.0 ) return false;

  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  const double distance = -m_localPlane.Distance(pLocal) / scalar;
  const Gaudi::XYZPoint hitInPanel( pLocal + distance*vLocal );

  if ( mode.detPlaneBound() == RichTraceMode::tight)
  {
    if ( fabs(hitInPanel.x()) >= m_detPlaneHorizEdge ||
         fabs(hitInPanel.y()) >= m_detPlaneVertEdge ) { return false; }
  }

  hitPosition = geometry()->toGlobal( hitInPanel );
  return true;
}
