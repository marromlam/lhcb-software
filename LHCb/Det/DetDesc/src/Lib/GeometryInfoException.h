#ifndef     DETDESC_GEOMETRYINFOEXCEPTION_H
#define     DETDESC_GEOMETRYINFOEXCEPTION_H 1 
/// GaudiKernel
#include "GaudiKernel/GaudiException.h" 
///
class MsgStream;
class GeometryInfo;
///

/** @class GeometryInfoException GeometryInfoException.h DetDesc/GeometryInfoException.h

    Exception class used in GeometryInfo objects 
    
    @author Vanya Belyaev 
*/

class GeometryInfoException : public GaudiException 
{  
public:
  /// constructor 
  GeometryInfoException( const std::string  & name     , 
                         const GeometryInfo * gi   = 0 ,
                         const StatusCode   & sc = StatusCode::FAILURE );
  ///
  GeometryInfoException( const std::string    & name      , 
                         const GaudiException & ge        , 
                         const GeometryInfo   * gi   =  0 ,
                         const StatusCode     & sc = StatusCode::FAILURE ) ;
  /// destructor 
  virtual ~GeometryInfoException();
  ///
  virtual std::ostream& printOut( std::ostream& os = std::cerr ) const ; 
  virtual MsgStream&    printOut( MsgStream&    os             ) const ;
  ///
  virtual GaudiException* clone() const;  
  ///
private:
  ///
  const GeometryInfo*   m_gie_geometryInfo  ; 
  ///
};
///

#endif  //  DETDESC_GEOMETRYINFOEXCEPTION_H

