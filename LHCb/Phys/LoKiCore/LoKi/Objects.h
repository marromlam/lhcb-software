// $Id: Objects.h,v 1.4 2006-02-18 18:06:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/02/10 17:23:05  ibelyaev
//  add more algorithms
//
// ============================================================================
#ifndef LOKI_OBJECTS_H 
#define LOKI_OBJECTS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <utility>
#include <functional>
// ============================================================================


// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-06 
 */
// ============================================================================


namespace LoKi 
{
  /** @namespace Objects Objects.h LoKi/Objects.h
   *
   *  Collection of useful small objects for LoKi 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-08
   */
  namespace Objects
  {
    
    /** @struct _ALL
     *  trivial class/functor/predicate which always evaluated to "true"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-08
     */
    template <class TYPE>
    struct _ALL  : public  std::unary_function<TYPE,bool>
    {
      inline bool operator() ( TYPE /* arg */ ) const { return true   ; }
    };
    
    /** @struct _NONE
     *  trivial class/functor/predicate which always evaluated to "false"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-08
     */
    template <class TYPE>
    struct _NONE : public  std::unary_function<TYPE,bool>
    {
      inline bool operator() ( TYPE /* arg */ ) const { return false  ; }
    };
    
    /** @struct _ONE
     *  trivial class/functor/predicate which always evaluated to "1"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-08
     */
    template <class TYPE>
    struct _ONE  : public  std::unary_function<TYPE,int>
    {
      inline int operator() ( TYPE /* arg */ ) const { return 1 ; }
    };

    /** @var _ALL_
     *  helper objects which allows the efficient implementation 
     *  of selection algorithms 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const _ALL<const void*> _ALL_ = _ALL<const void*>() ;
    
    /** @var _ONE_
     *  helper objects which allows the efficient implementation 
     *  of selection algorithms 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const _ONE<const void*> _ONE_ = _ONE<const void*>() ;
    
  } ; // end of the namespace LoKi::Objects 
  
} ; // end of the namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_OBJECTS_H
// ============================================================================
