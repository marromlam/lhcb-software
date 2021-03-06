// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles14.h"
#include "LoKi/Child.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
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
 *  @date 2006-02-22
 */
// ============================================================================
/*  constructor with daughter index (starts from 1).
 *  E.g. for 2-body decays it could be 1 or 2 
 *  @param index of daughter particles
 */
// ============================================================================
LoKi::Particles::DecayAngle::DecayAngle
( const size_t child ) 
  : LoKi::AuxFunBase ( std::tie ( child ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_child ( child ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::DecayAngle::DecayAngle
( const LoKi::Particles::DecayAngle& right ) 
  : LoKi::AuxFunBase                     ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function( right )
  , m_child                              ( right.m_child ) 
{}
// ============================================================================
LoKi::Particles::DecayAngle::result_type 
LoKi::Particles::DecayAngle::operator() 
  ( LoKi::Particles::DecayAngle::argument p ) const
{
  if ( 0 == p ) 
  { 
    Error ( "Argument is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  }               
  // get the gaughter particle
  const LHCb::Particle* child = LoKi::Child::child ( p , m_child ) ;
  if ( 0 == child ) 
  { 
    Error ( "'Child' is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle;                         // RETURN 
  }               
  const LoKi::LorentzVector& mv = p     -> momentum() ;
  const LoKi::LorentzVector& dv = child -> momentum() ;
  // 
  return LoKi::Kinematics::decayAngle( dv , mv ) ;
} 
// ============================================================================
// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayAngle::fillStream ( std::ostream& s ) const 
{ 
  s << "LV0" ;
  switch ( m_child ) 
  {
  case 1  :
    s << "1"                   ; break ;
  case 2  :
    s << "2"                   ; break ;
  case 3  :
    s << "3"                   ; break ;
  case 4  :
    s << "4"                   ; break ;
  default :
    s << "(" << m_child << ")" ; break ;
  }
  return s ;
}

// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ArmenterosPodolanski::result_type 
LoKi::Particles::ArmenterosPodolanski::operator() 
  ( LoKi::Particles::ArmenterosPodolanski::argument p ) const
{
  if ( 0 == p ) 
  { 
    Error ( " Argument is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  } 
  //
  if ( 2 != LoKi::Child::nChildren ( p ) ) 
  {
    Error ( " Invalid number of daughter particles, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  }
  // get the first  child 
  const LHCb::Particle* p1 = LoKi::Child::child ( p , 1 ) ;
  if ( 0 == p1 ) 
  {
    Error ( " Invalid first daughter particle, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  }
  // get the second child 
  const LHCb::Particle* p2 = LoKi::Child::child ( p , 2 ) ;
  if ( 0 == p2 ) 
  {
    Error ( " Invalid second daughter particle, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  }
  return LoKi::Kinematics::armenterosPodolanskiX 
    ( p1 -> momentum () , 
      p2 -> momentum () ) ; 
}



// ============================================================================
// The END 
// ============================================================================
