/// GaudiKernel
#include "GaudiKernel/ISolid.h"
#include "GaudiKernel/MsgStream.h"
/// DetDesc
#include "DetDesc/SolidException.h"
///

//////////////////////////////////////////////////////////////////////////////////////
SolidException::SolidException( const std::string& message  , 
                                const ISolid*      solid    ) 
  : GaudiException( message , "*SolidException*" , StatusCode::FAILURE ) 
  , m_se_solid    ( solid      ) 
{ } ;
///////////////////////////////////////////////////////////////////////////////////////
SolidException::SolidException( const std::string   &  message   ,
                                const GaudiException&  Exception ,  
                                const ISolid*          solid     ) 
  : GaudiException( message , "*SolidException*" , StatusCode::FAILURE , Exception ) 
  , m_se_solid    ( solid      ) 
{ } ;
///////////////////////////////////////////////////////////////////////////////////////
SolidException::~SolidException(){};
///////////////////////////////////////////////////////////////////////////////////////
std::ostream& SolidException::printOut( std::ostream& os             ) const 
{
  /// 
  os << " \t" << tag() << " \t " << message() ;       
  ///
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
    default                  : os << "\t StatusCode=" << code() ;  break ; 
    }
  ///
  os << " \tSolid=" <<  m_se_solid << std::endl ;
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) : os ;  
  ///
};
///////////////////////////////////////////////////////////////////////////////////////
MsgStream&    SolidException::printOut( MsgStream&    os             ) const 
{
  /// 
  os << " \t" << tag() << " \t " << message() ;       
  ///
  switch( code() )
    {
    case StatusCode::SUCCESS : os << "\t StatusCode=SUCCESS"    ;  break ; 
    case StatusCode::FAILURE : os << "\t StatusCode=FAILURE"    ;  break ; 
    default                  : os << "\t StatusCode=" << code() ;  break ; 
    }
  ///
  os << " \tSolid=" <<  m_se_solid << endreq;
  ///
  return ( 0 != previous() ) ? previous()->printOut( os ) :  os ;  
  ///
};
///////////////////////////////////////////////////////////////////////////////////////
GaudiException* SolidException::clone () const { return new SolidException(*this); }
///////////////////////////////////////////////////////////////////////////////////////


