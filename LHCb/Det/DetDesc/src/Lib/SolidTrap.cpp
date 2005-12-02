// $Id: SolidTrap.cpp,v 1.16 2005-12-02 18:36:56 jpalac Exp $
// ===========================================================================
/**  GaudiKernel package */  
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/StreamBuffer.h" 

/**  DetDesc package */ 
#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidTrap.h" 
#include "DetDesc/SolidBase.h"

// ============================================================================
/** @file SolidTrap.cpp 
 * 
 *  implementation of class SolidTrap
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name                   name of trap solid 
 *  @param zHalfLength            half length in z 
 *  @param theta                  theta angle 
 *  @param phi                    phi angle 
 *  @param dyAtMinusZ             delta y at bottom face
 *  @param dxAtMinusZMinusY       delta x at bottom face , minus y
 *  @param dxAtMinusZPlusY        delta x at bottom face , plus y 
 *  @param alphaAtMinusZ          alpha angle at bottom face 
 *  @param dyAtPlusZ              delta y at top face 
 *  @param dxAtPlusZMinusY        delta x at top face, minus y 
 *  @param dxAtPlusZPlusY         delta x at top face, plus y     
 *  @param alphaAtPlusZ           alpha angle at top face 
 *  @exception SolidException wrong parameters range 
 */   
// ============================================================================
SolidTrap::SolidTrap( const std::string&  Name             ,
                      const double        ZHalfLength      ,
                      const double        Theta            ,
                      const double        Phi              ,
                      const double        DyAtMinusZ       ,  
                      const double        DxAtMinusZMinusY , 
                      const double        DxAtMinusZPlusY  ,
                      const double        AlphaAtMinusZ    ,
                      const double        DyAtPlusZ        ,  
                      const double        DxAtPlusZMinusY  , 
                      const double        DxAtPlusZPlusY   ,
                      const double        AlphaAtPlusZ    )
  : SolidPolyHedronHelper   ( Name             ) 
  , SolidBase               ( Name             ) 
  , m_trap_zHalfLength      ( ZHalfLength      ) 
  , m_trap_theta            ( Theta            ) 
  , m_trap_phi              ( Phi              ) 
  , m_trap_dyAtMinusZ       ( DyAtMinusZ       ) 
  , m_trap_dxAtMinusZMinusY ( DxAtMinusZMinusY ) 
  , m_trap_dxAtMinusZPlusY  ( DxAtMinusZPlusY  ) 
  , m_trap_alphaAtMinusZ    ( AlphaAtMinusZ    ) 
  , m_trap_dyAtPlusZ        ( DyAtPlusZ        ) 
  , m_trap_dxAtPlusZMinusY  ( DxAtPlusZMinusY  ) 
  , m_trap_dxAtPlusZPlusY   ( DxAtPlusZPlusY   ) 
  , m_trap_alphaAtPlusZ     ( AlphaAtPlusZ     ) 
{
  if( 0 >= ZHalfLength      ) 
    { throw SolidException("SolidTrap::ZHalfLength is not positive!"   ); } 
  if( 0 >= DyAtMinusZ       ) 
     { throw SolidException("SolidTrap::dyAtMinusZ is not positive!"   ); } 
  if( 0 >= DxAtMinusZMinusY ) 
    { throw SolidException("SolidTrap::dxAtMinusZMinusY is not positive!");} 
  if( 0 >= DxAtMinusZPlusY  ) 
    { throw SolidException("SolidTrap::dxAtMinusZPlusY  is not positive!");} 
  if( 0 >= DyAtPlusZ        ) 
    { throw SolidException("SolidTrap::dyAtMinusZ is not positive!");} 
  if( 0 >= DxAtPlusZMinusY  ) 
    { throw SolidException("SolidTrap::dxAtMinusZMinusY is not positive!");} 
   if( 0 >= DxAtPlusZPlusY   ) 
     { throw SolidException("SolidTrap::dxAtMinusZPlusY is not positive!"); } 
   ///  
   makeAll();
   /// set bounding parameters
   setBP();
   
}; 

// ============================================================================
/** constructor 
 *  @param name name of general trapezoid 
 */
// ============================================================================
SolidTrap::SolidTrap( const std::string& Name )
  : SolidPolyHedronHelper   ( Name ) 
  , SolidBase               ( Name ) 
  , m_trap_zHalfLength      ( 100  ) 
  , m_trap_theta            ( 0    ) 
  , m_trap_phi              ( 0    ) 
  , m_trap_dyAtMinusZ       ( 100  ) 
  , m_trap_dxAtMinusZMinusY ( 100  ) 
  , m_trap_dxAtMinusZPlusY  ( 100  ) 
  , m_trap_alphaAtMinusZ    ( 0    ) 
  , m_trap_dyAtPlusZ        ( 100  ) 
  , m_trap_dxAtPlusZMinusY  ( 100  ) 
  , m_trap_dxAtPlusZPlusY   ( 100  ) 
  , m_trap_alphaAtPlusZ     ( 0    ) 
{};

// ============================================================================
/// destructror 
// ============================================================================
SolidTrap::~SolidTrap() {}

// ============================================================================
/** initialize the polihedron base 
 *  @exception SolidException wrong parameters  
 */
// ============================================================================
void SolidTrap::makeAll()
{   
  /// clear vertices 
  m_ph_vertices.clear();
  m_ph_planes.clear();
  SolidPolyHedronHelper::reset() ; 
  {
    /** construct points (vertoces)
     * code is copied from G4 
     */
    double fDz         = zHalfLength(); 
    double fTthetaCphi = tan( theta() ) * cos( phi() ); 
    double fTthetaSphi = tan( theta() ) * sin( phi() ); 
    double fDy1        = dyAtMinusZ()           ; 
    double fDx1        = dxAtMinusZMinusY()     ;
    double fDx2        = dxAtMinusZPlusY ()     ; 
    double fTalpha1    = tan( alphaAtMinusZ() ) ; 
    double fDy2        = dyAtPlusZ()            ; 
    double fDx3        = dxAtPlusZMinusY()      ;
    double fDx4        = dxAtPlusZPlusY ()      ; 
    double fTalpha2    = tan( alphaAtPlusZ() )  ; 
    ///
    Gaudi::XYZPoint p0( -fDz * fTthetaCphi - fDy1 * fTalpha1 - fDx1 , 
                   -fDz * fTthetaSphi - fDy1 , -fDz ) ;
    Gaudi::XYZPoint p1( -fDz * fTthetaCphi - fDy1 * fTalpha1 + fDx1 , 
                   -fDz * fTthetaSphi - fDy1 , -fDz ) ;
    Gaudi::XYZPoint p2( -fDz * fTthetaCphi + fDy1 * fTalpha1 - fDx2 , 
                   -fDz * fTthetaSphi + fDy1 , -fDz ) ;
    Gaudi::XYZPoint p3( -fDz * fTthetaCphi + fDy1 * fTalpha1 + fDx2 ,
                   -fDz * fTthetaSphi + fDy1 , -fDz ) ;
    Gaudi::XYZPoint p4( +fDz * fTthetaCphi - fDy2 * fTalpha2 - fDx3 ,
                   +fDz * fTthetaSphi - fDy2 , +fDz ) ;
    Gaudi::XYZPoint p5( +fDz * fTthetaCphi - fDy2 * fTalpha2 + fDx3 , 
                   +fDz * fTthetaSphi - fDy2 , +fDz ) ;
    Gaudi::XYZPoint p6( +fDz * fTthetaCphi + fDy2 * fTalpha2 - fDx4 ,
                   +fDz * fTthetaSphi + fDy2 , +fDz ) ;
    Gaudi::XYZPoint p7( +fDz * fTthetaCphi + fDy2 * fTalpha2 + fDx4 , 
                   +fDz * fTthetaSphi + fDy2 , +fDz ) ;
    ///
    m_ph_vertices.push_back( p0 ) ; 
    m_ph_vertices.push_back( p1 ) ; 
    m_ph_vertices.push_back( p2 ) ; 
    m_ph_vertices.push_back( p3 ) ; 
    m_ph_vertices.push_back( p4 ) ; 
    m_ph_vertices.push_back( p5 ) ; 
    m_ph_vertices.push_back( p6 ) ; 
    m_ph_vertices.push_back( p7 ) ; 
    ///
  }
  if( 8 != m_ph_vertices.size() ) 
    { throw SolidException("SolidTrap:: wrong vertex #!"); } 
  /// make faces
  addFace( point(0) , point(4) , point(5) , point(1) ) ;
  addFace( point(2) , point(3) , point(7) , point(6) ) ;
  addFace( point(0) , point(2) , point(6) , point(4) ) ;
  addFace( point(1) , point(5) , point(7) , point(3) ) ;
  addFace( point(0) , point(1) , point(3) , point(2) ) ; /// bottom face
  addFace( point(5) , point(4) , point(6) , point(7) ) ; /// top    face
  ///
  if( 6 != planes().size() ) 
    { throw SolidException("SolidTrap::wrong face # "); } 
  ///
};

// ============================================================================
/** - serialization for reading
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param      s               reference to stream buffer
 *  @exception  SolidException  wrong parameters range 
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidTrap::serialize( StreamBuffer& s ) 
{
  /// 
  reset();
  m_ph_planes.clear();
  /// serialize the base class 
  SolidBase::serialize( s ) ;
  /// serialize memebrs 
  s >> m_trap_zHalfLength    
    >> m_trap_theta           
    >> m_trap_phi              
    >> m_trap_dyAtMinusZ       
    >> m_trap_dxAtMinusZMinusY 
    >> m_trap_dxAtMinusZPlusY  
    >> m_trap_alphaAtMinusZ    
    >> m_trap_dyAtPlusZ        
    >> m_trap_dxAtPlusZMinusY  
    >> m_trap_dxAtPlusZPlusY   
    >> m_trap_alphaAtPlusZ ;    
  ///
  if( 0 >= zHalfLength()      ) 
    { throw SolidException("SolidTrap::ZHalfLength is not positive!");} 
  if( 0 >= dyAtMinusZ ()      ) 
    { throw SolidException("SolidTrap::dyAtMinusZ is not positive!"); } 
  if( 0 >= dxAtMinusZMinusY() )
    { throw SolidException("SolidTrap::dxAtMinusZMinusY is not positive!");} 
  if( 0 >= dxAtMinusZPlusY () ) 
    { throw SolidException("SolidTrap::dxAtMinusZPlusY is not positive!");} 
  if( 0 >= dyAtPlusZ       () ) 
    { throw SolidException("SolidTrap::dyAtMinusZ is not positive!");} 
  if( 0 >= dxAtPlusZMinusY () )
    { throw SolidException("SolidTrap::dxAtMinusZMinusY is not positive!");} 
  if( 0 >= dxAtPlusZPlusY  () ) 
    { throw SolidException("SolidTrap::dxAtMinusZPlusY is not positive!");} 
  ///
  makeAll();
  /// set bounding parameters
  setBP();
  return s;
  ///
};

// ============================================================================
/** - serialization for writing
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param s reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidTrap::serialize( StreamBuffer& s ) const 
{
  /// serialize base class 
  SolidBase::serialize( s );
  ///
  return 
    s << m_trap_zHalfLength    
      << m_trap_theta           
      << m_trap_phi              
      << m_trap_dyAtMinusZ       
      << m_trap_dxAtMinusZMinusY 
      << m_trap_dxAtMinusZPlusY  
      << m_trap_alphaAtMinusZ    
      << m_trap_dyAtPlusZ        
      << m_trap_dxAtPlusZMinusY  
      << m_trap_dxAtPlusZPlusY   
      << m_trap_alphaAtPlusZ  ; 
};

// ============================================================================
/** - retrieve the pointer to "simplified" solid - "cover"
 *    -# the cover for Trap is Trd 
 *  - implementation of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid*           SolidTrap::cover         () const 
{
  /// cover is calculated already 
  if( 0 != m_cover ) { return m_cover; }             
  ///
  double ymx1 = fabs( point(0).y() )  ;
  double xmx1 = fabs( point(0).x() ) ;
  double ymx2 = fabs( point(4).y() )  ;
  double xmx2 = fabs( point(4).x() ) ;
  ///  
  for( VERTICES::size_type i = 1 ; i <  4 ; ++i )
    {
      xmx1 = fabs( point(i).x() ) > xmx1 ? fabs( point(i).x() ) : xmx1 ; 
      ymx1 = fabs( point(i).y() ) > ymx1 ? fabs( point(i).y() ) : ymx1 ; 
    }  
  for( VERTICES::size_type i1 = 4 ; i1 <  8 ; ++i1 )
    {
      xmx2 = fabs( point(i1).x() ) > xmx2 ? fabs( point(i1).x() ) : xmx2 ; 
      ymx2 = fabs( point(i1).y() ) > ymx2 ? fabs( point(i1).y() ) : ymx2 ; 
    } 
  ///   
  ISolid* cov = new SolidTrd( "Cover for " + name  () , 
                              zHalfLength          () ,
                              xmx1         , ymx1     , 
                              xmx2         , ymx2     );
  m_cover = cov; 
  return m_cover;
};

// ============================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os STD/STL stream
 *  @return reference to the stream 
 */
// ============================================================================
std::ostream&  SolidTrap::printOut      ( std::ostream&  os ) const
{
  SolidBase::printOut( os );
  return 
    os << "["  
       << " sizeZ[mm]="   << zHalfLength      () / millimeter    
       << " theta[deg]="  << theta            () / degree 
       << " phi[deg]="    << phi              () / degree 
       << " sizeY1[mm]="  << dyAtMinusZ       () / millimeter 
       << " sizeX1[mm]="  << dxAtMinusZMinusY () / millimeter 
       << " sizeX2[mm]="  << dxAtMinusZPlusY  () / millimeter 
       << " alp1[deg]="   << alphaAtMinusZ    () / degree 
       << " sizeY2[mm]="  << dyAtPlusZ        () / millimeter 
       << " sizeX3[mm]="  << dxAtPlusZMinusY  () / millimeter 
       << " sizeX4[mm]="  << dxAtPlusZPlusY   () / millimeter 
       << " alp2[deg]="   << alphaAtPlusZ     () / degree  << "]";
};

// ============================================================================
/** - printout to Gaudi MsgStream stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( MsgStream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os Gaudi MsgStream  stream
 *  @return reference to the stream 
 */
// ============================================================================
MsgStream&     SolidTrap::printOut      ( MsgStream&     os ) const
{
  SolidBase::printOut( os );
  return 
    os << "["  
       << " sizeZ[mm]="   << zHalfLength      () / millimeter    
       << " theta[deg]="  << theta            () / degree 
       << " phi[deg]="    << phi              () / degree 
       << " sizeY1[mm]="  << dyAtMinusZ       () / millimeter 
       << " sizeX1[mm]="  << dxAtMinusZMinusY () / millimeter 
       << " sizeX2[mm]="  << dxAtMinusZPlusY  () / millimeter 
       << " alp1[deg]="   << alphaAtMinusZ    () / degree 
       << " sizeY2[mm]="  << dyAtPlusZ        () / millimeter 
       << " sizeX3[mm]="  << dxAtPlusZMinusY  () / millimeter 
       << " sizeX4[mm]="  << dxAtPlusZPlusY   () / millimeter 
       << " alp2[deg]="   << alphaAtPlusZ     () / degree  << "]";
};
// ============================================================================




