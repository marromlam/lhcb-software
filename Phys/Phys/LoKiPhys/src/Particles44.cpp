// $Id: Particles44.cpp 86202 2010-09-14 13:01:14Z ibelyaev $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GetAlgs.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleValue.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles44.h"
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
 *  @author Sebastian Neubert
 *  @date 2013-08-05
 */
// ============================================================================
// constructor from the tool name 
// ============================================================================
LoKi::Particles::Value::Value
( const std::string& function ) 
  : LoKi::AuxFunBase ( std::tie ( function ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_function ()
{
  // 1. get from the context 
  LoKi::ILoKiSvc* loki  = lokiSvc() ;
  //
  SmartIF<IAlgContextSvc> context ( loki ) ;
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( context ) ;
  if ( 0 != alg ) 
  { m_function = alg->tool<IParticleValue> ( function , alg , true ) ; }
  else 
  { 
    /// 2. use tool service 
    SmartIF<IToolSvc> tsvc ( loki ) ;
    if ( ! ( ! tsvc ) ) 
    {
      IParticleValue*   _func  = 0 ;
      const IInterface* parent = 0 ;
      StatusCode sc = tsvc->retrieveTool 
        ( function , _func , parent , true ) ;
      Assert ( sc.isSuccess() , "Unable to retrieve the tool " + function , sc ) ;
      Assert ( 0 != _func     , "Unable to retrieve the tool " + function      ) ;
      m_function = _func ;
    }
  }
  //
  Assert ( m_function.validPointer() , "Unable to locate tool " + function ) ;
} 
// ============================================================================
// constructor from the function 
// ============================================================================
LoKi::Particles::Value::Value
( const IParticleValue* function ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_function ( function )
{} 
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::Value::~Value()
{
  if ( m_function && !gaudi() ) 
  {
    // Warning("IParticleValue: manual reset") ;
    m_function.reset() ;
  }
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Value*
LoKi::Particles::Value::clone() const { return new Value(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::Value::result_type
LoKi::Particles::Value::operator() 
  ( LoKi::Particles::Value::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "Invalid argument, return 'false'");
    return false ;                                                 // RETURN 
  }
  if ( !m_function ) 
  {
    Error ( "Invalid tool: IParticleValue* points to NULL, return 'false'");
    return false ;                                                 // RETURN 
  }
  // use tool
  return m_function->operator()( p ) ;
} 
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::Value::fillStream( std::ostream& s ) const 
{
  s << " VALUE( ";
  if ( m_function.validPointer() ) 
  { s << m_function->type() << "/" << m_function->name() ; }
  else 
  { s << "<invalid>" ; }
  return s << " ) " ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================

