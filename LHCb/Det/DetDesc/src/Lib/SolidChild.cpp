/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// CLHEP
#include "CLHEP/Geometry/Transform3D.h" 
#include "CLHEP/Geometry/Point3D.h" 
/// GaudiKernel
#include "GaudiKernel/IInspector.h" 
#include "GaudiKernel/StreamBuffer.h"
/// DetDesc 
#include "DetDesc/SolidChild.h" 
#include "DetDesc/ClhepToStream.h" 
#include "DetDesc/SolidFactory.h"
#include "DetDesc/Solid.h"

/// ============================================================================
/** @file SolidChild.cpp 
 *  
 *  implementation of class SolidChild
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxxx
 */
/// ============================================================================

/// ============================================================================
/// factory business 
/// ============================================================================
static const  SolidFactory<SolidChild>         s_Factory;
const        ISolidFactory&SolidChildFactory = s_Factory;

/// ============================================================================
/** constructor
 *  @param Name name of this solid 
 */
/// ============================================================================
SolidChild::SolidChild( const std::string& Name )
  : SolidBase      ( Name  )
  , m_sc_solid     (  0    )
  , m_sc_matrix    (  0    )
  , m_sc_simple    ( true  )
{};

/// ============================================================================
/** constructor    
 *  @param solid pointer to ISolid object
 *  @param mtrx  pointer to transformation 
 *  @param Name name of this solid 
 */ 
/// ============================================================================
SolidChild::SolidChild( ISolid*               solid , 
                        const HepTransform3D* mtrx  ,
                        const std::string&    Name  )
  : SolidBase      ( Name  ) 
  , m_sc_solid     ( solid )
  , m_sc_matrix    (   0   ) 
  , m_sc_simple    ( true  )
{
  if( 0 == solid ) 
    { throw SolidException("SolidChild(), ISolid* points to NULL!");}
  ///
  if( 0 != mtrx && !(HepTransform3D::Identity == *mtrx) )
    { m_sc_matrix    = new HepTransform3D( *mtrx ); 
    if( 0 != m_sc_matrix) { m_sc_simple = false ; } }
  else
    { m_sc_simple = true  ; }
};

/// ============================================================================
/** constructor 
 *  @param solid pointer ot ISolid object
 *  @param pos   position
 *  @param rot   rotation
 *  @param Name name of this solid 
 */
/// ============================================================================
SolidChild::SolidChild( ISolid*               solid ,
                        const HepPoint3D&     pos   , 
                        const HepRotation&    rot   ,
                        const std::string&    Name  ) 
  : SolidBase      ( Name  ) 
  , m_sc_solid     ( solid )
  , m_sc_matrix    (   0   ) 
  , m_sc_simple    ( true  )
{
  ////
  if( 0 == solid ) 
    { throw SolidException("SolidChild(), ISolid* points to NULL!");}
  ///
  const HepPoint3D NullPoint(0,0,0);
  ///
  if( NullPoint == pos && rot.isIdentity() ) 
    { m_sc_simple = true  ; }         
  else 
    {
      m_sc_matrix    = 
        new HepTransform3D( HepRotate3D( rot ) * HepTranslate3D( -1.0 * pos ) );
      if( 0 != m_sc_matrix ) { m_sc_simple = false ; } 
    }
};

/// ============================================================================
/// destructor 
/// ============================================================================
SolidChild::~SolidChild()
{ 
  reset(); 
  if( 0 != m_sc_solid  ){ delete m_sc_solid ; m_sc_solid  = 0; } 
  if( 0 != m_sc_matrix ){ delete m_sc_matrix; m_sc_matrix = 0; } 
};

/// ============================================================================
/** serialization for reading
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
/// ============================================================================
StreamBuffer& SolidChild::serialize( StreamBuffer& sb ) 
{
  ///
  reset();
  /// serialise the base class 
  SolidBase::serialize( sb );
  /// serialize the solid
  std::string type;
  sb >> type;
  if( 0 != m_sc_solid ){ delete m_sc_solid ; m_sc_solid = 0; }
  m_sc_solid = Solid::createSolid( type );
  m_sc_solid->serialize( sb );
  char c;
  sb >> c;
  m_sc_simple = c ? true : false ;
  if( 0 != m_sc_matrix ) { delete m_sc_matrix ; m_sc_matrix = 0 ; }
  if( !m_sc_simple ) 
    { m_sc_matrix = new HepTransform3D() ; sb >> (*m_sc_matrix); }
  ///
  return sb;
};

/// ============================================================================
/** serialization for writing
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
/// ============================================================================
StreamBuffer& SolidChild::serialize( StreamBuffer& sb ) const
{  
  /// serialise the base class 
  SolidBase::serialize( sb );
  /// serialise the solid 
  sb << m_sc_solid->typeName() ;
  solid()->serialize( sb );
  /// serialize the matrix
  sb << (char) m_sc_simple;
  if( !m_sc_simple ) { sb <<  *m_sc_matrix ; }
  ///
  return sb;
};

/// ============================================================================
/** printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
/// ============================================================================
std::ostream& SolidChild::printOut     ( std::ostream& os ) const
{
  SolidBase::printOut( os );
  return os << solid(); 
};

/// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
/// ============================================================================
MsgStream&    SolidChild::printOut     ( MsgStream&    os ) const
{
  SolidBase::printOut( os );
  return os << solid(); 
};

/// ============================================================================
/** - calculate the intersection points("ticks") of the solid objects 
 *    with given line. 
 *  -# Line is parametrized with parameter \a t :
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
 *      - \f$ \vec{p} \f$ is a point on the line 
 *      - \f$ \vec{v} \f$ is a vector along the line  
 *  -# \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
 *    (\f$\vec{v}\f$) are defined in local reference system 
 *   of the solid 
 *  - implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
/// ============================================================================
unsigned int 
SolidChild::intersectionTicks ( const HepPoint3D&  Point  ,
                                const HepVector3D& Vector ,
                                ISolid::Ticks&     ticks  ) const
{
  return solid()->
    intersectionTicks(  simple() ? Point  : matrix() * Point  , 
                        simple() ? Vector : matrix() * Vector , 
                        ticks                                 ) ;
};

/// ============================================================================
/** reset to the initial ("after constructor") state
 */
/// ============================================================================
const ISolid*  SolidChild::reset()
{
  SolidBase::reset();
  if( 0 != solid() ){ m_sc_solid->reset() ; } ; 
  return this ; 
};

/// ============================================================================
/** - check for the given 3D-point. 
 *    Point coordinated are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
/// ============================================================================
bool SolidChild::isInside ( const HepPoint3D& point) const 
{ 
  return  
    ( simple() ? solid()->isInside(point) : 
      solid()->isInside( matrix() * point ) ) ; 
}; 

/// ============================================================================
