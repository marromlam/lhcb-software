// $Id: NsctVeloTransporter.cpp,v 1.3 2005-02-25 13:28:58 pkoppenb Exp $
// Include files 
#include "CLHEP/Matrix/SymMatrix.h"  // here because of CLHEP bug

// Utility Classes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SmartDataPtr.h"

// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"

// Detector description classes
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"

// local
#include "NsctVeloTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NsctVeloTransporter
//
// 22/02/2002 : Edgar de Oliveira
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<NsctVeloTransporter>          s_factory ;
const        IToolFactory& NsctVeloTransporterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NsctVeloTransporter::NsctVeloTransporter( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool( type, name , parent ) 
  , m_tolerance(1.e-5)
  , m_k(0.01){
  

  // declare additional Interface
  declareInterface<IParticleTransporter>(this);
  
  return ;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode NsctVeloTransporter::initialize(){
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;
  debug() << "NsctVeloTransporter Initialization starting..." << endreq;
  
  debug() << "Retrieving now detector elements" << endreq;
  IDetectorElement* vertex = get<IDetectorElement>( detSvc(),
                                                    "/dd/Structure/LHCb/Velo" );
  if( !vertex ) {
    err()  << "Can't retrieve /dd/Structure/LHCb/Velo" << endreq;
    return StatusCode::FAILURE;
  }
  
  const ILVolume* ivol = vertex->geometry()->lvolume();
  if (!ivol){
    err() << "No volume vertex->geometry()->lvolume()" << endreq;
    return StatusCode::FAILURE;    
  }
  unsigned long noppv = ivol->noPVolumes();
  
  // starting noppv 2 = vStation00
  // m_vStations = noppv - 2;
  
  // m_zVelo = z position of velo stations
  unsigned int ppvc;  
  // filling the Velo z position
  for (ppvc = 2; ppvc < noppv; ppvc++) {
    HepTransform3D vtran =  ivol->pvolume(ppvc)->matrixInv();
    
    Hep3Vector vvec = vtran.getTranslation();
    m_zVelo.push_back( vvec.z() );
    
  }
  return StatusCode::SUCCESS;  
}
//=============================================================================
//  Transport linear + multiple scattering (particle iterator)
//=============================================================================
StatusCode NsctVeloTransporter::transport(ParticleVector::const_iterator &icand, 
                                          const double znew,
                                          Particle & transParticle){
  
  const Particle workParticle = *(*icand);  
  
  return transport(workParticle,znew,transParticle);    
}
//=============================================================================
//  Transport linear + multiple scattering (const particle)
//=============================================================================
StatusCode NsctVeloTransporter::transport(const Particle & icand, 
                                          const double znew,
                                          Particle & transParticle){

  const Particle *workParticle = &icand;
  
  // check z-range
  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  double zold = oldPOT.z();

  int ipz = 1;
  double zr = zold;
  double zl = znew;
  if ( znew > zold ) {
    ipz = -1;
    zr = znew;
    zl = zold;
  }
  
  if ( zl < -500.0 || zr > 21000.0 ){
    debug() << " z is out of range, z < -500.0 or z > 21000.0" 
        << endreq;    
    return StatusCode::FAILURE;
  }
  
  // ipz = 1 upstream going tracks; ipz = -1 downstream going tracks
  
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = this->veloTransport(workParticle, znew, transParticle, ipz);
  return sc;    
}
//=============================================================================
//  Transport linear + multiple scattering ( common calculation )
//=============================================================================
StatusCode NsctVeloTransporter::veloTransport(const Particle *&workParticle, 
                                             const double znew, 
                                             Particle &transParticle,
                                             int ipz){
  
  // check for particle
  if ( !workParticle ) {
    warning() 
        << "NsctVeloTransporter::transport should be called with a"
        << "pointer to a particle as argument!" << endreq;
    return StatusCode::FAILURE;
  }

  // initialization of transported Particle  
  transParticle = *workParticle;
  
  // initialize and create PointOnTrack vectors  
  HepPoint3D oldPOT(999., 999., 999.);
  oldPOT = workParticle->pointOnTrack();
  HepPoint3D newPOT = oldPOT;
  
  // initialize and create PointOnTrack matrix error
  HepSymMatrix oldPOTErr(3, 0);
  oldPOTErr = workParticle->pointOnTrackErr();
  
  // initialize and create Slope and Momentum correlation matrix error 
  HepSymMatrix oldSlopesMomErr(3, 0);
  oldSlopesMomErr = workParticle->slopesMomErr();
  
  // initialize and create Position, Slope and Momentum correlation 
  // matrix error
  HepMatrix oldPosSlopesCorr(3, 3, 0);
  oldPosSlopesCorr = workParticle->posSlopesCorr();

  // transform to the same z with err_z=0
  HepMatrix JA(3,3);
  JA(1,1)=1;
  JA(1,2)=0;
  JA(1,3)=-workParticle->slopeX();
  JA(2,1)=0;
  JA(2,2)=1;
  JA(2,3)=-workParticle->slopeY();
  JA(3,1)=0;
  JA(3,2)=0;
  JA(3,3)=0;
  oldPOTErr=oldPOTErr.similarity(JA);
  oldPosSlopesCorr=oldPosSlopesCorr*JA.T();

  // initialize and create new matrices
  // notice z-related elements are zero in newPOTErr and newPosSlopesCorr
  HepSymMatrix newPOTErr = oldPOTErr;
  HepSymMatrix newSlopesMomErr = oldSlopesMomErr;
  HepMatrix newPosSlopesCorr = oldPosSlopesCorr;
  
  // check current z-position
  double zold = oldPOT.z();
  double dz = znew - zold;
  if (fabs(dz) < m_tolerance){
    // already at required z position
    debug() << " already at required a position " << endreq;
    return StatusCode::SUCCESS;
  }
  // x and y coordinates at point on track
  double xold = oldPOT.x();
  double yold = oldPOT.y();
  
  double sx = workParticle->slopeX();
  double sy = workParticle->slopeY();
  
  // counting how many stations are on the way 
  // if there is none, the transport will be linear
  
  int nscat = 0;
  double innerR = 8.0;
  double outerR = 42.0;
  double zr;
  double zl;
  zr = zold;
  zl = znew;
  if ( ipz == -1 ){ zr = znew; zl = zold;  }
  // upstream/downstream search
  std::vector<double>::const_iterator iStep;
  for ( iStep = m_zVelo.begin(); iStep != m_zVelo.end();
        iStep++ ){
    if ( *(iStep) < zr && *(iStep) > zl ) {
      // track distance from the beam at the station
      double xts = xold + ( *(iStep) - zold )*sx;
      double yts = yold + ( *(iStep) - zold )*sy;
      double rts =sqrt ( xts*xts + yts*yts );
      if ( innerR < rts && outerR > rts ) nscat = nscat + 1;
    }
  }
  
  // linear transport
  double dz2 = dz*dz;
  double xnew = xold + sx*dz;
  double ynew = yold + sy*dz;
  newPOT.set(xnew, ynew, znew);
  
  newPOTErr(1,1) = oldPOTErr(1,1) + 2.*dz*oldPosSlopesCorr(1,1) + 
    dz2*oldSlopesMomErr(1,1);    
  newPOTErr(2,1) = oldPOTErr(2,1) + 
    dz*( oldPosSlopesCorr(1,2) + oldPosSlopesCorr(2,1) ) + 
    dz2*oldSlopesMomErr(2,1);
  newPOTErr(2,2) = oldPOTErr(2,2) + 2.*dz*oldPosSlopesCorr(2,2) + 
    dz2*oldSlopesMomErr(2,2);
  newPosSlopesCorr(1,1) = oldPosSlopesCorr(1,1) + dz*oldSlopesMomErr(1,1);
  newPosSlopesCorr(2,2) = oldPosSlopesCorr(2,2) + dz*oldSlopesMomErr(2,2);
  newPosSlopesCorr(1,2) = oldPosSlopesCorr(1,2) + dz*oldSlopesMomErr(1,2);
  newPosSlopesCorr(2,1) = oldPosSlopesCorr(2,1) + dz*oldSlopesMomErr(2,1);  

  // transport considers multiple scattering 
  double a = 1. / ( sqrt (1.0 + sx*sx + sy*sy ) ); 
  double p = workParticle->p();
  if ( nscat > 0 && workParticle->charge()!=0.0) {    
    double b =  sx*a;
    double c =  sy*a;
    HepVector3D UnitVector(b, c, a);
    double CosTheta = UnitVector.z();
    double X0 = nscat /(fabs(CosTheta));
    
    double Teth = (m_k/p)*sqrt(X0);
    newSlopesMomErr(1,1) = oldSlopesMomErr(1,1) + 
      pow( Teth*(1. + sx*sx), 2);
    newSlopesMomErr(2,2) = oldSlopesMomErr(2,2) + 
      pow( Teth*(1. + sy*sy), 2);
    newPOTErr(1,1) = oldPOTErr(1,1) + 2.*dz*oldPosSlopesCorr(1,1) + 
      dz2*oldSlopesMomErr(1,1);    
    newPOTErr(2,1) = oldPOTErr(2,1) +
      dz*( oldPosSlopesCorr(1,2) + oldPosSlopesCorr(2,1) ) +
      dz2*oldSlopesMomErr(2,1);
    newPOTErr(2,2) = oldPOTErr(2,2) + 2.*dz*oldPosSlopesCorr(2,2) + 
      dz2*oldSlopesMomErr(2,2);
    newPosSlopesCorr(1,1) = oldPosSlopesCorr(1,1) + dz*oldSlopesMomErr(1,1);
    newPosSlopesCorr(2,2) = oldPosSlopesCorr(2,2) + dz*oldSlopesMomErr(2,2);
    newPosSlopesCorr(1,2) = oldPosSlopesCorr(1,2) + dz*oldSlopesMomErr(1,2);
    newPosSlopesCorr(2,1) = oldPosSlopesCorr(2,1) + dz*oldSlopesMomErr(2,1);
  }
  
  // a new "particle" is made with transported values
  transParticle.setPointOnTrack(newPOT);
  transParticle.setPointOnTrackErr(newPOTErr);
  transParticle.setSlopesMomErr(newSlopesMomErr);
  transParticle.setPosSlopesCorr(newPosSlopesCorr);
  return StatusCode::SUCCESS;  
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode NsctVeloTransporter::finalize() {
  
  debug() << "==> NsctVeloTransporter::finalize" << endreq;
  m_zVelo.clear();
  
  return GaudiTool::finalize();
}

