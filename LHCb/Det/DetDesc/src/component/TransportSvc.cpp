
/// from Gaudi 
#include "GaudiKernel/Kernel.h" 
#include "GaudiKernel/SvcFactory.h" 

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"


#include "GaudiKernel/IGeometryInfo.h" 
#include "GaudiKernel/IDetectorElement.h" 

#include "GaudiKernel/ISolid.h" 
#include "GaudiKernel/ILVolume.h" 
#include "GaudiKernel/IPVolume.h" 

#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h" 

///
/// from CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"


#include "DetDesc/TransportSvc.h" 
#include "DetDesc/VolumeIntersectionIntervals.h" 


/// local 
#include "TransportSvcDistanceInRadUnits.h" 
#include "TransportSvcFindLocalGI.h" 
#include "TransportSvcGoodLocalGI.h" 
#include "TransportSvcIntersections.h" 


///
///
///  TransportSvc.cpp implementation of class TransportSvc  
///
///  Author: Vanya Belyaev
///
///


/// Instantiation of a static factory class used by clients to create
/// instances of this service
static const  SvcFactory<TransportSvc>                       s_factory ;
extern const ISvcFactory&              TransportSvcFactory = s_factory ;
/// Constructor 
TransportSvc::TransportSvc( const std::string& name, ISvcLocator* ServiceLocator )
  : Service                       ( name , ServiceLocator ) 
  /// names of "standard" services
  , m_chronoStatSvc_name          (  "ChronoStatSvc"      )
  , m_magFieldSvc_name            (  "MagneticFieldSvc"   )
  , m_detDataSvc_name             (  "DetectorDataSvc"    )
  /// pointers to "standard" services
  , m_chronoStatSvc               (          0            ) 
  , m_magFieldSvc                 (          0            )
  , m_detDataSvc                  (          0            )  
  /// source of "standard" geometry 
  , m_standardGeometry_address    (  "/dd/Structure/LHCb" ) 
  , m_standardGeometry            (          0            ) 
  ///
  , m_previousGeometry            (          0            )
  /// "cache" parameters 
  , m_prevPoint1                  (                       ) 
  , m_prevPoint2                  (                       ) 
  , m_previousThreshold           (        -10000         ) 
  , m_previousGuess               (          0            ) 
  , m_previousAlternative         (          0            ) 
  , m_localIntersections          (                       )
  ///
  , m_local_intersects            (                       )
{
  /// properties 
  declareProperty( "ChronoStatService"    , m_chronoStatSvc_name       ) ; 
  declareProperty( "MagneticFieldService" , m_magFieldSvc_name         ) ; 
  declareProperty( "DetectorDataService"  , m_detDataSvc_name          ) ; 
  declareProperty( "StandardGeometryTop"  , m_standardGeometry_address ) ;
};
///
/// Destructor:  
TransportSvc::~TransportSvc(){};
/// Implementation of IInterface::queryInterface()
StatusCode    TransportSvc::queryInterface(const IID& riid, void** ppvInterface)
{
  ///
  if ( IID_ITransportSvc == riid ) { *ppvInterface = (ITransportSvc*) this                 ; }
  else                             { return Service::queryInterface( riid , ppvInterface ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;
  ///
};
/// Implementation of initialize() method 
StatusCode TransportSvc::initialize()
{
  MsgStream log( msgSvc() , name() + ".initialize()" );  
  {
    /// initialise the base class 
    StatusCode statusCode = Service::initialize() ;  
    if( statusCode.isFailure() ) 
      { log << MSG::ERROR << " Unable to initialize base class! " << endreq; return statusCode ; }              /// RETURN !!!
  }
  /// Set my own properties
  setProperties();  
  /// locate services to be used:
  { 
    /// 1) locate Chrono & Stat Service: 
    StatusCode statusCode = serviceLocator()->service( m_chronoStatSvc_name , m_chronoStatSvc ) ;    
    /// it is not a fatal error!!!
    if( statusCode.isSuccess() && 0 != m_chronoStatSvc ) { chronoSvc()->addRef(); } 
    else { log << MSG::WARNING << " Unable to locate  Chrono & Stat Service=" << m_chronoStatSvc_name << endreq; m_chronoStatSvc = 0 ; }  
  }
  ///
  { 
    /// 2) locate MagneticField Service: 
    StatusCode statusCode = serviceLocator()->service( m_magFieldSvc_name , m_magFieldSvc ) ;    
    /// it is a fatal error!!!
    if( statusCode.isSuccess() && 0 != m_magFieldSvc ) { magFieldSvc()->addRef(); } 
    else 
      { 
        log << MSG::FATAL << " Unable to locate  Magnetic Field Service=" << m_magFieldSvc_name << endreq; 
        m_magFieldSvc = 0 ;  
        return StatusCode::FAILURE ;                                                                   /// RETURN !!! 
      } 
  }
  ///
  { 
    /// 3) locate Detector Data  Service: 
    StatusCode statusCode = serviceLocator()->service( m_detDataSvc_name , m_detDataSvc ) ;
    /// it is a fatal error!!!
    if( statusCode.isSuccess() && 0 != m_detDataSvc ) { detSvc()->addRef(); } 
    else 
      { 
        log << MSG::FATAL << " Unable to locate  Detector Data Service=" << m_detDataSvc_name << endreq; 
        m_magFieldSvc = 0 ;  
        return StatusCode::FAILURE ;                                                                   /// RETURN !!! 
      } 
  }
  /// locate other services: 
  ///
  return StatusCode::SUCCESS;
  ///
};
/// Implementation of finalize() method 
StatusCode TransportSvc::finalize()
{
  MsgStream log ( msgSvc() , name() ); 
  {
    /// release all services 
    MsgStream log  ( msgSvc() , name() + ".finalize()" ); 
    /// release Chrono & Stat Service 
    if( 0 != chronoSvc   () ) { chronoSvc()->release()    ; m_chronoStatSvc    = 0 ; } 
    /// release Magnetic Field Service 
    if( 0 != magFieldSvc () ) { magFieldSvc()->release()  ; m_magFieldSvc      = 0 ; } 
    /// release Detector Data  Service 
    if( 0 != detSvc      () ) { detSvc()->release()       ; m_detDataSvc       = 0 ; } 
    /// finalize the base class 
    StatusCode statusCode  = Service::finalize(); 
    if( statusCode.isFailure() ) { return statusCode; }                                                 /// RETURN !!!
  }  
  ///
  log << MSG::DEBUG << "Service finalised successfully" << endreq;
  ///  
  return StatusCode::SUCCESS;
  ///
};
///
IGeometryInfo*       TransportSvc::findGeometry( const std::string& address ) const 
{
  ///
  IGeometryInfo* gi = 0 ; 
  ///
  try 
    {
      SmartDataPtr<IDetectorElement> detelem( detSvc() , address ); 
      if( 0 != detelem ) { gi = detelem->geometry() ; } 
    }
  catch(...) { Assert( false , "TransportSvc::findGeometry(), unknown exception caught!" ); }  
  ///
  Assert( 0 != gi , "TransportSvc::unable to locate geometry address="+address );
  ///
  return gi;
};



