
/** @file DeRich1.cpp
 *
 *  Implementation file for detector description class : DeRich1
 *
 *  CVS Log :-
 *  $Id: DeRich1.cpp,v 1.10 2004-10-18 09:21:49 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.9  2004/09/01 15:20:19  papanest
 *  added functions for TabProps
 *
 *  Revision 1.8  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH1_CPP

// Include files
#include "RichDet/DeRich1.h"
#include "RichDet/DeRichFlatMirror.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID& CLID_DERich1 = 12001;  // User defined

// Standard Constructors
DeRich1::DeRich1() {}

// Standard Destructor
DeRich1::~DeRich1() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1::classID() {
  return CLID_DERich1;
}

//===========================================================================

StatusCode DeRich1::initialize() 
{

  MsgStream log(msgSvc(), "DeRich1" );
  log << MSG::DEBUG << "Starting initialisation for DeRich1" << endreq;

  StatusCode sc = StatusCode::SUCCESS;
  StatusCode fail = StatusCode::FAILURE;

  if ( !DeRich::initialize() ) return fail;

  double nominalCoCX = userParameterAsDouble("Rich1Mirror1NominalCoCX");
  double nominalCoCY = userParameterAsDouble("Rich1Mirror1NominalCoCY");
  double nominalCoCZ = userParameterAsDouble("Rich1Mirror1NominalCoCZ");

  m_nominalCentreOfCurvature =
    HepPoint3D(nominalCoCX, nominalCoCY, nominalCoCZ);
  m_nominalCentreOfCurvatureBottom =
    HepPoint3D(nominalCoCX, -nominalCoCY, nominalCoCZ);

  //  std::vector<double> nominalCoC = paramVector("Rich1NominalCoC");
  //  m_nominalCentreOfCurvature =
  //    HepPoint3D( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  //  m_nominalCentreOfCurvatureBottom =
  //    HepPoint3D( nominalCoC[0], -nominalCoC[1], nominalCoC[2]);

  log << MSG::DEBUG << "Nominal centre of curvature"
      << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureBottom
      << endmsg;

  m_sphMirrorRadius = paramAsDouble("Rich1SphMirror1Radius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane = paramVector("Rich1NominalFlatMirrorPlane");
  m_nominalPlaneTop = HepPlane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                 nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneBottom = HepPlane3D(nominalFMirrorPlane[0],-nominalFMirrorPlane[1],
                                    nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalPlaneTop.normalize();
  m_nominalPlaneBottom.normalize();
  m_nominalNormal = m_nominalPlaneTop.normal();
  m_nominalNormalBottom = m_nominalPlaneBottom.normal();

  log << MSG::DEBUG << "Nominal normal " << HepVector3D( m_nominalNormal )
      << HepVector3D( m_nominalNormalBottom ) << endmsg;

  const IPVolume* pvGasWindow = geometry()->lvolume()->
    pvolume("pvRich1SubMaster")->lvolume()->pvolume("pvRich1MagShH0:0")->
    lvolume()->pvolume("pvRich1GQuartzWH0:0");

  if ( pvGasWindow ) {
    const Material::Tables& quartzWinTabProps = pvGasWindow->lvolume()->
      material()->tabulatedProperties();
    Material::Tables::const_iterator matIter;
    for (matIter=quartzWinTabProps.begin(); matIter!=quartzWinTabProps.end(); ++matIter) {
      if( (*matIter) ){
        if ( (*matIter)->type() == "RINDEX" ) {
          m_gasWinRefIndex = (*matIter);
        }
        if ( (*matIter)->type() == "ABSLENGTH" ) {
          m_gasWinAbsLength = (*matIter);
        }
      }
    }
  } else {
    log << MSG::ERROR << "Could not find gas window properties" << endreq;
    return fail;
  }
  
  std::string HPD_QETabPropLoc;
  
  if ( hasParam( "RichHpdQETableName" ) )
    HPD_QETabPropLoc = paramAsString( "RichHpdQETableName" );
  else
    HPD_QETabPropLoc = "/dd/Materials/RichMaterialTabProperties/HpdQuantumEff";
  
  SmartDataPtr<TabulatedProperty> tabQE (dataSvc(), HPD_QETabPropLoc);
  if ( !tabQE )
    log << MSG::ERROR << "No info on HPD Quantum Efficiency" << endmsg;
  else
    m_HPDQuantumEff = tabQE;

  log << MSG::DEBUG << "Finished initialisation for DeRich1" << endreq;
  return sc;
}
