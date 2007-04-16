// $Id: AlgoTypes.h,v 1.2 2007-04-16 16:16:39 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/04/09 08:51:49  ibelyaev
//  update for LoKiAlgo
//
// ============================================================================
#ifndef LOKI_ALGOTYPES_H 
#define LOKI_ALGOTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/NamedRange.h"
#include "LoKi/RangeList.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-23 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace AlgoTypes AlgoTypes.h LoKi/AlgoTypes.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-23
   */
  namespace AlgoTypes 
  {
    /// the actual type of RangeList  
    typedef LoKi::RangeList_<LoKi::Types::Range>   RangeList ;
  } ; // end of namespace LoKi::AlgoTypes 
  namespace Types
  {
    /// the actual type of RangeList  
    typedef LoKi::AlgoTypes::RangeList             RangeList ;    
  } ; // end of namespace LoKi::Types
} ; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGOTYPES_H
// ============================================================================
