// Include files
#include <numeric>
#include <algorithm>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CaloKernel/CaloCellID.h"
#include "CaloKernel/CaloTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
//#include "Event/CaloDigitStatus.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "Event/CaloDataFunctor.h"

#include "CaloSCorrectionFinal.h"

#include "math.h"

// ============================================================================
/** @file CaloSCorrectionFinal.cpp
 *
 *  Implementation file for class : CaloSCorrectionFinal
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   14/11/2002
 */
// ============================================================================


#ifdef WIN32
namespace std
{
    template <class T>
		inline const T& max( const T& a , const T& b )
	{ return (a<b) ? b : a ;};

   template <class T>
		inline const T& min( const T& a , const T& b )
	{ return (a<b) ? a : b ;};

};
#endif


// ============================================================================
/** @var CaloSCorrectionFinalFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSCorrectionFinal>         s_factory ;
const        IToolFactory&CaloSCorrectionFinalFactory = s_factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance
 *  @param parent  pointer to parent object (algorithm, service or tool)
 */
// ============================================================================
CaloSCorrectionFinal::CaloSCorrectionFinal(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : CaloTool( type, name, parent )
  , m_Coeff_area_0_X()
  , m_Coeff_area_0_Y()
  , m_Coeff_area_1_X()
  , m_Coeff_area_1_Y()
  , m_Coeff_area_2_X()
  , m_Coeff_area_2_Y()
  , m_Coeff_border_area_0_X()
  , m_Coeff_border_area_0_Y()
  , m_Coeff_border_area_1_X()
  , m_Coeff_border_area_1_Y()
  , m_Coeff_border_area_2_X()
  , m_Coeff_border_area_2_Y() 
  ///
  , m_a2GeV        ()
  , m_b2           ()
  , m_s2gain       ()
  , m_s2incoherent ()
  , m_s2coherent   ()
  ///
  , m_resA         ( 0.10 ) // calorimeter resolution (stochastic term)
  , m_resB         ( 0.01 ) // calorimeter resolution (constant term)
  , m_gainS        ( 0.01 ) // relative variantion of gain
//  , m_sigmaIn      ( 1.30 ) // sigma of incoherent noise (in channels)
  , m_sigmaIn      ( 0.00 ) // sigma of incoherent noise (in channels)
//  , m_sigmaCo      ( 0.30 ) // sigma of coherent noise
  , m_sigmaCo      ( 0.00 ) // sigma of coherent noise
{
  declareInterface<ICaloHypoTool>(this);
  declareProperty("Coeff0X",m_Coeff_area_0_X);
  declareProperty("Coeff0Y",m_Coeff_area_0_Y);
  declareProperty("Coeff1X",m_Coeff_area_1_X);
  declareProperty("Coeff1Y",m_Coeff_area_1_Y);
  declareProperty("Coeff2X",m_Coeff_area_2_X);
  declareProperty("Coeff2Y",m_Coeff_area_2_Y);
  declareProperty("Coeff0Xborder",m_Coeff_border_area_0_X);
  declareProperty("Coeff0Yborder",m_Coeff_border_area_0_Y);
  declareProperty("Coeff1Xborder",m_Coeff_border_area_1_X);
  declareProperty("Coeff1Yborder",m_Coeff_border_area_1_Y);
  declareProperty("Coeff2Xborder",m_Coeff_border_area_2_X);
  declareProperty("Coeff2Yborder",m_Coeff_border_area_2_Y);
  //
  declareProperty("ResolutionA"        , m_resA     ) ;
  declareProperty("ResolutionB"        , m_resB     ) ;
  declareProperty("RelativeGainError"  , m_gainS    ) ;
  declareProperty("NoiseIncoherent"    , m_sigmaIn  ) ;
  declareProperty("NoiseCoherent"      , m_sigmaCo  ) ;
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloSCorrectionFinal::~CaloSCorrectionFinal() {}
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode CaloSCorrectionFinal::initialize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "intialize() has been called" << endreq;
  /// initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) {
    Error("Could not initialize the base class ",sc);
    return StatusCode::FAILURE;
  }
  ///
  if( 0 == det() ) {
    if( 0 == detSvc() ) {
      Error("Detector Data Service is invalid!");
      return StatusCode::FAILURE;
    }
    ///
    SmartDataPtr<DeCalorimeter> calo( detSvc() , detName() );
    if( !calo ) {
      Error("Could not locate detector='"+detName()+"'");
      return StatusCode::FAILURE;
    }
    /// set detector
    setDet( calo );
  }
  if( 0 == det() ) {
    Error("DeCalorimeter* points to NULL!");
    return StatusCode::FAILURE;
  }
  /// coeffs initialisation
  if (m_Coeff_area_0_X.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_0_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_0_Y.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_0_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_1_X.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_1_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_1_Y.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_1_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_2_X.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_2_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_2_Y.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_2_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
	
	m_Coeff_X[0] = m_Coeff_area_0_X;
	m_Coeff_Y[0] = m_Coeff_area_0_Y;
	m_Coeff_X[1] = m_Coeff_area_1_X;
	m_Coeff_Y[1] = m_Coeff_area_1_Y;
	m_Coeff_X[2] = m_Coeff_area_2_X;
	m_Coeff_Y[2] = m_Coeff_area_2_Y;

  if (m_Coeff_border_area_0_X.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_0_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_0_Y.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_0_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_1_X.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_1_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_1_Y.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_1_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_2_X.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_2_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_2_Y.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_2_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
	
	m_Coeff_border_X[0] = m_Coeff_border_area_0_X;
	m_Coeff_border_Y[0] = m_Coeff_border_area_0_Y;
	m_Coeff_border_X[1] = m_Coeff_border_area_1_X;
	m_Coeff_border_Y[1] = m_Coeff_border_area_1_Y;
	m_Coeff_border_X[2] = m_Coeff_border_area_2_X;
	m_Coeff_border_Y[2] = m_Coeff_border_area_2_Y;
	

  m_a2GeV        = m_resA    * m_resA    * GeV  ;
  m_b2           = m_resB    * m_resB           ;
  m_s2gain       = m_gainS   * m_gainS          ;
  m_s2incoherent = m_sigmaIn * m_sigmaIn        ;
  m_s2coherent   = m_sigmaCo * m_sigmaCo        ;

	msg << MSG::INFO << "m_a2GeV:" << m_a2GeV << endreq;
	msg << MSG::INFO << "m_b2:" << m_b2 << endreq;

  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode CaloSCorrectionFinal::finalize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "finalize() has been called" << endreq;
  /// remove detector
  setDet( (const DeCalorimeter*) 0 );
  /// finalize the  the base class
  return CaloTool::finalize ();
}
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code
 */
// ============================================================================
StatusCode CaloSCorrectionFinal::process( CaloHypo* hypo ) const
{ return (*this) ( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code
 */
// ============================================================================
StatusCode CaloSCorrectionFinal::operator() ( CaloHypo* hypo ) const {

  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "process() has been called" << endreq;
	int i,j,k,l;

	StatusCode update_data = StatusCode::SUCCESS;

  if (hypo==0) {
		msg << MSG::ERROR << "hypo==0" << endreq;
		return StatusCode::FAILURE;
	}
  if (hypo->hypothesis()!=CaloHypotheses::Photon) {
		msg << MSG::ERROR << "hypo->hypothesis()!=CaloHypotheses::Photon" << endreq;
		//return StatusCode::FAILURE;
	}
  SmartRefVector<CaloCluster> clusters = hypo->clusters();
  if (clusters.size()!=1) {
		msg << MSG::ERROR << "clusters.size()!=1" << endreq;
		return StatusCode::FAILURE;
	}
  CaloCluster *cluster;
  for (SmartRef<CaloCluster> *clusterloop = clusters.begin();
       clusterloop!=clusters.end();
       ++clusterloop) {
    if (clusterloop!=0) {cluster = *(clusterloop);}
  }
  std::vector<CaloClusterEntry> sac = cluster->entries();
  double maxenergy=0.;
  SmartRef<CaloDigit> seed;
  msg << MSG::VERBOSE << "looping on digits:" << endreq;
	CaloClusterEntry* cce;
  for (cce=sac.begin();cce!=sac.end();++cce) {
    if (cce==0) {continue;}
    SmartRef<CaloDigit> srcd=cce->digit();
    if (srcd==0) {continue;}
    if (srcd->e()*cce->fraction()>maxenergy) {
      maxenergy=srcd->e()*cce->fraction();
      seed=srcd;
    }
    msg << MSG::VERBOSE << "e()= " << srcd->e()*cce->fraction() << endreq;
  }
  if (seed==0) {
		msg << MSG::ERROR << "seed==0" << endreq;
		return StatusCode::FAILURE;
	}
  if (maxenergy==0) {
		msg << MSG::ERROR << "maxenergy==0" << endreq;
		return StatusCode::FAILURE;
	}
  msg << MSG::VERBOSE << "seed energy: " << seed->e() << endreq;
  msg << MSG::VERBOSE << "seed cell: " << seed->cellID() << endreq;
  double xseed=det()->cellX(seed->cellID())*mm;
  msg << MSG::VERBOSE << "seed cell x: " << xseed << endreq;
  double yseed=det()->cellY(seed->cellID())*mm;
  msg << MSG::VERBOSE << "seed cell y: " << yseed << endreq;
  int rowseed=seed->cellID().row();
  msg << MSG::VERBOSE << "seed cell raw: " << rowseed << endreq;
  int colseed=seed->cellID().col();
  msg << MSG::VERBOSE << "seed cell col: " << colseed << endreq;
  unsigned int areaseed=seed->cellID().area();
  msg << MSG::VERBOSE << "seed cell area: " << areaseed << endreq;
  if (areaseed>2) {
		msg << MSG::ERROR << "areaseed>2" << endreq;
		update_data = StatusCode::FAILURE;
		//return StatusCode::FAILURE;
  }
  double sizeseed=det()->cellSize(seed->cellID())*mm;
  msg << MSG::VERBOSE << "seed cell size: " << sizeseed << endreq;
  if (sizeseed<0.1) {
		msg << MSG::ERROR << "sizeseed<0.1" << endreq;
		update_data = StatusCode::FAILURE;
		//return StatusCode::FAILURE;
  }

	double E[3][3];
	double cov_ii[3][3];
	double gain[3][3];
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {
			E[i][j]=0.0;
			cov_ii[i][j]=0.0;
			gain[i][j]=0.0;
		}
	}
	for (cce=sac.begin();cce!=sac.end();++cce) {
		if (cce==0) {continue;}
		SmartRef<CaloDigit> srcd=cce->digit();
		if (srcd==0) {continue;}
		if (srcd->cellID().area()!=areaseed) {continue;}
		int row=srcd->cellID().row()-rowseed+1;
		int col=srcd->cellID().col()-colseed+1;
		if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
			E[col][row]=srcd->e()*cce->fraction();
			// intrinsic resolution
			cov_ii[col][row] = fabs(srcd->e()*cce->fraction())*m_a2GeV;
			if( 0 != m_b2 ) {
				cov_ii[col][row] += srcd->e()*cce->fraction()*srcd->e()*cce->fraction()*m_b2;
			}
			//  gain fluctuation
			if( 0 != m_s2gain) {
				cov_ii[col][row] += srcd->e()*cce->fraction()*srcd->e()*cce->fraction()*m_s2gain;
			}
			//  noise (both coherent and incoherent)
			if( 0 != (m_s2coherent + m_s2incoherent) ) {
				gain[col][row]   = det()->cellGain(srcd->cellID());
				cov_ii[col][row] += (m_s2coherent + m_s2incoherent)*gain[col][row]*gain[col][row] ;
			}
		}
	}

	// non diagonal covariance allocation (must be symetric)
	double cov_ij[3][3][3][3];
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					cov_ij[i][j][k][l]=0.;
				}
			}
		}
	}
	for (cce=sac.begin();cce!=sac.end();++cce) {
		if (cce==0) {continue;}
		SmartRef<CaloDigit> srcd=cce->digit();
		if (srcd==0) {continue;}
		int row=srcd->cellID().row()-rowseed+1;
		int col=srcd->cellID().col()-colseed+1;
		if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
			E[col][row]=srcd->e()*cce->fraction();
			if( 0 == m_s2coherent ) {continue;}
			CaloClusterEntry* cce2;
			for (cce2=sac.begin();cce2!=sac.end();++cce2) {
				if (cce2==0) {continue;}
				SmartRef<CaloDigit> srcd2=cce2->digit();
				if (srcd2==0) {continue;}
				int row_l=srcd2->cellID().row()-rowseed+1;
				int col_l=srcd2->cellID().col()-colseed+1;
				if ((row_l>=0)&&(row_l<=2)&&(col_l>=0)&&(col_l<=2)) {
					if ((row_l!=row)&&(col_l!=col)) {
						cov_ij[col][row][col_l][row_l] = m_s2coherent*gain[col][row]*gain[col_l][row_l];
					}
				}
			}
		}
	}


	for (i=0;i<3;i++) {
		msg << MSG::VERBOSE << "|" << E[2][i] << "|" << E[1][i] << "|" << E[0][i] << endreq;
	}

	bool border=false;
	int numberofneighbor = 0;
	const CaloCellID* cellloop;
	for (cellloop = det()->neighborCells(seed->cellID()).begin();
		cellloop!=det()->neighborCells(seed->cellID()).end();
		++cellloop) {
		numberofneighbor++;
		if (areaseed!=cellloop->area()) {border=true;}
	}
	if (numberofneighbor!=8) {border=true;}

	double energy=0.;
	double eleft=0.,evert=0.,eright=0.;
	double ebottom=0.,ehori=0.,etop=0.;
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {energy+=E[i][j];}
	}
	msg << MSG::VERBOSE << "energy: " << energy << endreq;

	eleft  =E[0][0]+E[0][1]+E[0][1];
	evert  =E[1][0]+E[1][1]+E[1][1];
	eright =E[2][0]+E[2][1]+E[2][1];
	ebottom=E[0][0]+E[1][0]+E[2][0];
	ehori  =E[0][1]+E[1][1]+E[2][1];
	etop   =E[0][2]+E[1][2]+E[2][2];

	bool borderx=false;
	bool bordery=false;
	if (border) {
		if ((eleft==0.)||(eright==0.)) {borderx=true;}
		if ((ebottom==0.)||(etop==0.)) {bordery=true;}
		if ((borderx==false)&&(bordery==false)) {
			msg << MSG::ERROR << "(border)&&(borderx==false)&&(bordery==false)" << endreq;
			update_data = StatusCode::FAILURE;
		}
	}
	msg << MSG::VERBOSE << "borderx: " << borderx << endreq;
	msg << MSG::VERBOSE << "bordery: " << bordery << endreq;

	double EDiffX[3][3],EDiffY[3][3];
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {EDiffX[i][j]=0.0;EDiffY[i][j]=0.0;}
	}

	double x=0.,param_x=0.;
	if (!borderx) {
		param_x=m_Coeff_X[areaseed][2]
			+m_Coeff_X[areaseed][3]*fabs(xseed)
			+m_Coeff_X[areaseed][4]*log(energy)
			+m_Coeff_X[areaseed][5]*log(energy)*log(energy);
		const double Enum = (eright-eleft);
		const double Edenom = (eright+m_Coeff_X[areaseed][1]*evert+eleft);
		x=m_Coeff_X[areaseed][0]*Enum/Edenom;
		x=param_x*asinh(2*x*sinh(1./param_x))/2.;
		const double Difffactor = x*2.*m_Coeff_X[areaseed][0]*sinh(1./param_x);
		// in eright
		EDiffX[2][0]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffX[2][1]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffX[2][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		// in evert
		EDiffX[1][0]=Difffactor*-m_Coeff_X[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffX[1][1]=Difffactor*-m_Coeff_X[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffX[1][2]=Difffactor*-m_Coeff_X[areaseed][1]*Enum/(Edenom*Edenom);
		// in eleft
		EDiffX[0][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffX[0][1]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffX[0][2]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		msg << MSG::VERBOSE << "X standart correction..." << endreq;
	} else {
		param_x=m_Coeff_border_X[areaseed][0];
		const double Enum = (eright-eleft);
		const double Edenom = (eright+m_Coeff_X[areaseed][1]*evert+eleft);
		x=Enum/Edenom;
		x=param_x*asinh(2*x*sinh(1./param_x))/2.;
		const double Difffactor = x*2.*sinh(1./param_x);
		// in eright
		EDiffY[2][0]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][1]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		// in evert
		EDiffY[1][0]=Difffactor*-Enum/(Edenom*Edenom);
		EDiffY[1][1]=Difffactor*-Enum/(Edenom*Edenom);
		EDiffY[1][2]=Difffactor*-Enum/(Edenom*Edenom);
		// in eleft
		EDiffY[0][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[0][1]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[0][2]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		msg << MSG::VERBOSE << "X border correction..." << endreq;
	}
	x*=sizeseed;
	x+=xseed;

	double y=0.,param_y=0.;
	if (!bordery) {
		param_y=m_Coeff_Y[areaseed][2]
			+m_Coeff_Y[areaseed][3]*fabs(xseed)
			+m_Coeff_Y[areaseed][4]*log(energy)
			+m_Coeff_Y[areaseed][5]*log(energy)*log(energy);
		const double Enum = (etop-ebottom);
		const double Edenom = (etop+m_Coeff_Y[areaseed][1]*ehori+ebottom);
		y=m_Coeff_Y[areaseed][0]*Enum/Edenom;
		y=param_y*asinh(2*y*sinh(1./param_y))/2.;
		const double Difffactor = y*2.*m_Coeff_Y[areaseed][0]*sinh(1./param_y)*sizeseed;
		// in etop
		EDiffY[0][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[1][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		// in ehori
		EDiffY[0][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffY[1][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffY[2][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		// in ebottom
		EDiffY[0][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[1][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		msg << MSG::VERBOSE << "Y standart correction..." << endreq;
	} else {
		param_y=m_Coeff_border_Y[areaseed][0];
		const double Enum = (etop-ebottom);
		const double Edenom = (etop+m_Coeff_Y[areaseed][1]*ehori+ebottom);
		y=Enum/Edenom;
		y=param_y*asinh(2*y*sinh(1./param_y))/2.;
		const double Difffactor = y*2.*m_Coeff_Y[areaseed][0]*sinh(1./param_y)*sizeseed;
		// in etop
		EDiffY[0][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[1][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		// in ehori
		EDiffY[0][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffY[1][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffY[2][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		// in ebottom
		EDiffY[0][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[1][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		msg << MSG::VERBOSE << "Y border correction..." << endreq;
	}
	y*=sizeseed;
	y+=yseed;

	// update cluster parameters
	{ // V.B.
	  if( 0 == hypo->position() )
	    { hypo->setPosition( cluster->position().clone() ) ; }
	}

	if (update_data == StatusCode::SUCCESS ) {
		CaloPosition::Parameters& parameters = hypo->position()->parameters();
		// keep old energy from ECorr
		//parameters( CaloPosition::E ) = energy ;
		parameters( CaloPosition::X ) = x ;
		parameters( CaloPosition::Y ) = y ;
		msg << MSG::VERBOSE << "X/Y/E updated..." << endreq;


		for (i=0;i<3;i++) {
			for (j=0;j<3;j++) {EDiffX[i][j]=1.0;EDiffY[i][j]=1.0;}
		}

		double SumEE =0.;
		double SumEX =0.;
		double SumEY =0.;
		double SumXX =0.;
		double SumXY =0.;
		double SumYY =0.;
		for (i=0;i<3;i++) {
			for (j=0;j<3;j++) {
				double xi = xseed+(double)(i-1)*sizeseed;
				double yj = yseed+(double)(j-1)*sizeseed;
				SumEE+=cov_ii[i][j];
				SumEX+=cov_ii[i][j]*EDiffX[i][j]*xi;
				SumEY+=cov_ii[i][j]*EDiffY[i][j]*yj;
				SumXX+=cov_ii[i][j]*EDiffX[i][j]*xi*EDiffX[i][j]*xi;
				SumXY+=cov_ii[i][j]*EDiffX[i][j]*xi*EDiffY[i][j]*yj;
				SumYY+=cov_ii[i][j]*EDiffY[i][j]*yj*EDiffY[i][j]*yj;
				// i*m sure that:
				// [i][j][i][j] = 0.
				// [i][j][k][l] = [k][l][i][j]
				for (k=0;k<=i;k++) {
					for (l=0;l<=j;l++) {
						if ((i==k)&&(j==l)) {continue;}
						double xk = xseed+(double)(k-1)*sizeseed;
						double yl = yseed+(double)(l-1)*sizeseed;
						SumEE+=cov_ij[i][j][k][l];
						SumEX+=cov_ij[i][j][k][l]*(EDiffX[i][j]*xi+EDiffX[k][l]*xk)/2.;
						SumEY+=cov_ij[i][j][k][l]*(EDiffY[i][j]*yj+EDiffY[k][l]*yl)/2.;
						SumXX+=cov_ij[i][j][k][l]*EDiffX[i][j]*xi*EDiffX[k][l]*xk;
						SumXY+=cov_ij[i][j][k][l]*(EDiffX[i][j]*xi*EDiffY[k][l]*yl+EDiffY[i][j]*yj*EDiffX[k][l]*xk)/2.;
						SumYY+=cov_ij[i][j][k][l]*EDiffY[i][j]*yj*EDiffY[k][l]*yl;
					}
				}
			}
		}

		// from V.B.
		double CovEE = SumEE;
		double CovEX = SumEX/energy - x*SumEE/energy;
		double CovEY = SumEY/energy - y*SumEE/energy;
		double CovXX = SumXX/energy/energy + x*x*SumEE/energy/energy - 2.*x*SumEX/energy/energy;
		double CovXY = SumXY/energy/energy + x*y*SumEE/energy/energy - y*SumEX/energy/energy - x*SumEY/energy/energy;
		double CovYY = SumYY/energy/energy + y*y*SumEE/energy/energy - 2.*y*SumEY/energy/energy;

		CaloPosition::Covariance& covariance = hypo->position()->covariance();
		msg << MSG::VERBOSE << "covariance EE:" << covariance( CaloPosition::E , CaloPosition::E ) << endreq;
		msg << MSG::VERBOSE << "COV        EE:" << CovEE << endreq;
		msg << MSG::VERBOSE << "covariance EX:" << covariance( CaloPosition::E , CaloPosition::X ) << endreq;
		msg << MSG::VERBOSE << "COV        EX:" << CovEX << endreq;
		msg << MSG::VERBOSE << "covariance EY:" << covariance( CaloPosition::E , CaloPosition::Y ) << endreq;
		msg << MSG::VERBOSE << "COV        EY:" << CovEY << endreq;
		msg << MSG::VERBOSE << "covariance XX:" << covariance( CaloPosition::X , CaloPosition::X ) << endreq;
		msg << MSG::VERBOSE << "COV        XX:" << CovXX << endreq;
		msg << MSG::VERBOSE << "covariance XY:" << covariance( CaloPosition::X , CaloPosition::Y ) << endreq;
		msg << MSG::VERBOSE << "COV        XY:" << CovXY << endreq;
		msg << MSG::VERBOSE << "covariance YY:" << covariance( CaloPosition::Y , CaloPosition::Y ) << endreq;
		msg << MSG::VERBOSE << "COV        YY:" << CovYY << endreq;
		msg << MSG::VERBOSE << "covariance updated..." << endreq;
	} else {
	  msg << MSG::INFO << "something wrong detected, hypo position NOT updated..." << endreq;
	}

/*
  // X/Y/E: pass data to CaloPosition
  CaloPosition* position = new CaloPosition();
  //CaloPosition* position  = hypo->position();
  msg << MSG::VERBOSE << "CaloPosition created..." << endreq;
  HepVector localposition(3);
  localposition(CaloPosition::X)=x;
  localposition(CaloPosition::Y)=y;
  //localposition(CaloPosition::E)=energy;
  position->setParameters(localposition);
  msg << MSG::VERBOSE << "X/Y/E updated..." << endreq;
*/
//  HepSymMatrix localcovariance(3,1);
//  position->setCovariance(localcovariance);
  //HepSymMatrix localcovariance((hypo->position())->covariance());
  /*
  localcovariance(CaloPosition::E,CaloPosition::E)=
    ((hypo->position())->covariance())(CaloPosition::E,CaloPosition::E);
  localcovariance(CaloPosition::E,CaloPosition::X)=xprime
    *((hypo->position())->covariance())(CaloPosition::E,CaloPosition::X);
  localcovariance(CaloPosition::E,CaloPosition::Y)=yprime
    *((hypo->position())->covariance())(CaloPosition::E,CaloPosition::Y);
  localcovariance(CaloPosition::X,CaloPosition::X)=xprime*xprime
    *((hypo->position())->covariance())(CaloPosition::X,CaloPosition::X);
  localcovariance(CaloPosition::X,CaloPosition::Y)=xprime*yprime
    *((hypo->position())->covariance())(CaloPosition::Y,CaloPosition::Y);
  localcovariance(CaloPosition::Y,CaloPosition::Y)=yprime*yprime
    *((hypo->position())->covariance())(CaloPosition::Y,CaloPosition::Y);
    */
  /*
  //CaloPosition *old = hypo->position();
  //HepSymMatrix oldcov = ((hypo->position())->covariance());
  //HepSymMatrix cov = (hypo->position())->covariance();
  CaloPosition* old = hypo->position();

  //CaloPosition::Covariance oldcov(hypo->position()->covariance());
  msg << MSG::VERBOSE
         << ":" << position->covariance()
         << ":" << old->covariance()
         << endreq;
  */

/*
  // I need to find correct math formulas...
  double CovXX=1.;
  double CovXY=1.;
  double CovEX=1.;
  double CovYY=1.;
  double CovEY=1.;
  double CovEE=1.;
  // update cluster matrix
  CaloPosition::Covariance& covariance = hypo->position()->covariance();
  covariance( CaloPosition::X , CaloPosition::X ) = CovXX ;
  covariance( CaloPosition::Y , CaloPosition::X ) = CovXY ;
  covariance( CaloPosition::E , CaloPosition::X ) = CovEX ;
  covariance( CaloPosition::Y , CaloPosition::Y ) = CovYY ;
  covariance( CaloPosition::E , CaloPosition::Y ) = CovEY ;
  covariance( CaloPosition::E , CaloPosition::E ) = CovEE ;

*/

  msg << MSG::VERBOSE << "Covariance updated..." << endreq;
  //hypo->setPosition(position);
  msg << MSG::VERBOSE << "CaloHypo updated..." << endreq;

  return StatusCode::SUCCESS;
}
