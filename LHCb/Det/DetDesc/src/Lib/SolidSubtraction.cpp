/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
///@{
/** STD & STL */
#include <iostream> 
#include <string>
///@} 
///@{
/** DetDesc */
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidException.h"
#include "DetDesc/SolidFactory.h"
#include "DetDesc/Solid.h"
///@} 

/// ===========================================================================
/** @file SolidSubtraction 
 *
 *  implementation of class SolidSubtraction 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
/// ===========================================================================

/// ===========================================================================
/// factory business 
/// ===========================================================================
static const SolidFactory<SolidSubtraction>         s_Factory;
const       ISolidFactory&SolidSubtractionFactory = s_Factory;

/// ===========================================================================
/** constructor 
 *  @param name name of the intersection
 *  @param first pointer to first/main solid 
 */
/// ===========================================================================
SolidSubtraction::SolidSubtraction( const std::string& name  , 
                                    ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) 
    { throw SolidException(" SolidSubtraction:: ISolid* points to NULL! "); }
};

/// ===========================================================================
/** constructor 
 *  @param name name of the solid subtraction  
 */
/// ===========================================================================
SolidSubtraction::SolidSubtraction( const std::string& Name)
  : SolidBoolean( "Name" )
{};

/// ===========================================================================
/// destructor 
/// ===========================================================================
SolidSubtraction::~SolidSubtraction(){ reset(); }

/// ===========================================================================
/** - check for the given 3D-point. 
 *    Point coordinates are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
/// ===========================================================================
bool SolidSubtraction::isInside     ( const HepPoint3D   & point ) const 
{ 
  ///  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; }
  /// find the first daughter in which the given point is placed   
  SolidSubtraction::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , Solid::IsInside( point ) );
  ///
  return ( childEnd() == ci ? true : false );   
};

/// ===========================================================================
/** subtract child solid to the solid
 *  @param solid pointer to child solid 
 *  @param mtrx  pointer to transformation 
 *  @return status code 
 */
/// ===========================================================================
StatusCode  SolidSubtraction::subtract( ISolid*                solid    , 
                                        const HepTransform3D*  mtrx     )
{  return addChild( solid , mtrx ); };

/// ===========================================================================
/** subtract child solid from  the solid 
 *  @param solid pointer to child solid 
 *  @param position position  
 *  @return status code 
 */
/// ===========================================================================
StatusCode  SolidSubtraction::subtract ( ISolid*               solid    , 
                                         const HepPoint3D&     position , 
                                         const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; }; 

/// ===========================================================================
