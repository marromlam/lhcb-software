// $Id: PrintDecay.h,v 1.1 2006-05-27 11:44:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_PRINTDECAY_H 
#define LOKI_PRINTDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <iostream>
// ===========================================================================
// Event
// ===========================================================================
#include "Event/Particle.h"
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/PhysTypes.h"
// ===========================================================================
// forward declarations 
// ===========================================================================
class MsgStream ;
// ===========================================================================

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
 *  with the campain of <XXXXXXX> et al. : 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-26 
 */
// ============================================================================

namespace LoKi
{
  /** @fn printDecay
   *  Simple function to print decay in more or less "readable" format 
   *
   *  @code
   *
   *  const LHCb::Particle* p = ... ;
   * 
   *  printDecay( p , std::cout ) ;
   *
   *  @endcode 
   *
   *  @param particle pointer to particle to be printed 
   *  @param stream   stream to be used 
   *  @param cut      condition 
   *  @param blank    to be printed instead of cutted particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-01-18
   */
  std::ostream& printDecay 
  ( const LHCb::Particle*        particle                    , 
    std::ostream&                stream                      , 
    const LoKi::Types::Cuts&     cut   = 
    LoKi::BooleanConstant<const  LHCb::Particle*> ( true )   , 
    const std::string&           blank = "<cut>"             ) ;
  
  /** @fn printDecay
   *  Simple function to print decay in more or less "readable" format 
   *
   *  @code
   *
   *  const LHCb::Particle* p = ... ;
   * 
   *  printDecay( p , always() ) ;
   *
   *  @endcode 
   *
   *  @param particle pointer to Particle to be printed 
   *  @param stream   stream to be used 
   *  @param cut      condition 
   *  @param blank    to be printed instead of cutted particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-01-18
   */
  MsgStream& printDecay 
  ( const LHCb::Particle*        particle                    , 
    MsgStream&                   stream                      , 
    const LoKi::Types::Cuts&     cut   = 
    LoKi::BooleanConstant<const  LHCb::Particle*> ( true )   , 
    const std::string&           blank = "<cut>"             ) ;
  
  /** @fn printDecay
   *  Simple function to print decay in more or less "readable" format 
   *
   *  @code
   *
   *  const LHCb::Particle* p = ... ;
   * 
   *  info() << printDecay( p ) ;
   *
   *  @endcode 
   *
   *  @param particle pointer to Particle to be printed 
   *  @param cut      condition 
   *  @param blank    to be printed instead of cutted particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-01-18
   */
  std::string printDecay 
  ( const LHCb::Particle*        particle                    , 
    const LoKi::Types::Cuts&     cut   = 
    LoKi::BooleanConstant<const  LHCb::Particle*> ( true )   , 
    const std::string&           blank = "<cut>"             ) ;
  
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRINTDECAY_H
// ============================================================================
