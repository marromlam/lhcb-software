// $Id: Particles26.cpp,v 1.1 2009-04-24 12:49:05 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/GetDVAlgorithm.h"
#include "Kernel/DVAlgorithm.h"
/// ===========================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Child.h"
#include "LoKi/Particles26.h"
// ============================================================================
/** @file 
 *  Implementation file for functions form file LoKi/Particles26
 *  @date 2009-04-24 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// the invalid particle:
  const LHCb::Particle*      const s_PARTICLE = 0 ;
  // ==========================================================================
  /// the invalid tool
  const IDistanceCalculator* const s_TOOL     = 0 ;
  // ==========================================================================
  inline const IDistanceCalculator* getDC
  ( const std::string&      nick ,
    const LoKi::AuxFunBase& base )
  {
    // get LoKi service
    const LoKi::Interface<LoKi::ILoKiSvc>& svc = base.lokiSvc() ;
    base.Assert( !(!svc) , "LoKi Service is not available!" ) ;
    // get DVAlgorithm
    DVAlgorithm* alg = Gaudi::Utils::getDVAlgorithm
      ( SmartIF<IAlgContextSvc>( svc ) ) ;
    base.Assert ( 0 != alg , "DVAlgorithm is not available" ) ;
    const IDistanceCalculator* dc = alg->distanceCalculator( nick ) ;
    if ( 0 == dc )
    { base.Error("IDistanceCalculator('"+nick+"') is not available") ; }
    return dc ;
  }
  // ==========================================================================
  /// the the valid tool name
  inline std::string toolName
  ( const IAlgTool*    tool ,
    const std::string& nick )
  {
    if ( 0 == tool || !nick.empty() ) { return nick ; }
    //
    const std::string& name = tool -> name() ;
    const bool pub = ( 0 == name.find ("ToolSvc.") ) ;
    const std::string::size_type ldot = name.rfind ('.') ;
    //
    if ( std::string::npos != ldot )
    {
      return !pub ?
        tool -> type () + "/" + std::string ( name , ldot ) :
        tool -> type () + "/" + std::string ( name , ldot ) + ":PUBLIC" ;
    }
    //
    return tool->type() ;
  }
  // ==========================================================================
  /// the the valid tool name
  inline std::string toolName
  ( const LoKi::Interface<IDistanceCalculator>& dc   ,
    const std::string&                          nick )
  {
    const IAlgTool* tool = dc.getObject() ;
    return toolName ( tool , nick ) ;
  }
  // ==========================================================================
}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCA::DOCA
( const size_t               i1 ,
  const size_t               i2 ,
  const IDistanceCalculator* dc )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_eval ( s_PARTICLE , dc )
  , m_first  ( i1 )
  , m_second ( i2 )
  , m_nick   ("")
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCA::DOCA
( const size_t                                i1 ,
  const size_t                                i2 ,
  const LoKi::Interface<IDistanceCalculator>& dc )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_eval   ( s_PARTICLE , dc )
  , m_first  ( i1 )
  , m_second ( i2 )
  , m_nick   ( "" )
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCA::DOCA
( const size_t       i1 ,
  const size_t       i2 ,
  const std::string& nick )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_eval   ( s_PARTICLE , s_TOOL )
  , m_first  ( i1   )
  , m_second ( i2   )
  , m_nick   ( nick )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCA::result_type
LoKi::Particles::DOCA::operator()
  ( LoKi::Particles::DOCA::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  /// get the first daughter
  const LHCb::Particle* first = LoKi::Child::child( p , firstIndex() ) ;
  if ( 0 == first ) 
  {
    Error ("Invalid first daughter particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  /// get the second daughter
  const LHCb::Particle* second = LoKi::Child::child( p , secondIndex() ) ;
  if ( 0 == second ) 
  {
    Error ("Invalid second daughter particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  // tool is valid?
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result
  return doca ( first , second ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCA::fillStream ( std::ostream& s ) const
{ return s << "DOCA(" << firstIndex() << "," << secondIndex() << ",'"
           << toolName () << "')" ; }
// ============================================================================
// get toolname 
// ============================================================================
std::string LoKi::Particles::DOCA::toolName() const 
{ return ::toolName ( tool() , nickname() ) ; }
// ============================================================================
StatusCode LoKi::Particles::DOCA::loadTool () const 
{
  // tool is valid?
  if ( !tool() )
  {
    const IDistanceCalculator* dc = getDC ( nickname() , *this ) ;
    /// finally set the tool
    setTool ( dc ) ;
  }
  Assert( !(!tool()) , "Uanbel to locate tool!" ) ;
  //
  return StatusCode::SUCCESS ;
}




// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2::DOCAChi2
( const size_t               i1 ,
  const size_t               i2 ,
  const IDistanceCalculator* dc )
  : LoKi::Particles::DOCA( i1 , i2 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2::DOCAChi2
( const size_t                                i1 ,
  const size_t                                i2 ,
  const LoKi::Interface<IDistanceCalculator>& dc )
  : LoKi::Particles::DOCA( i1 , i2 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCAChi2::DOCAChi2
( const size_t       i1 ,
  const size_t       i2 ,
  const std::string& nick )
  : LoKi::Particles::DOCA( i1 , i2 , nick ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCAChi2::result_type
LoKi::Particles::DOCAChi2::operator()
  ( LoKi::Particles::DOCAChi2::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  /// get the first daughter
  const LHCb::Particle* first = LoKi::Child::child( p , firstIndex() ) ;
  if ( 0 == first ) 
  {
    Error ("Invalid first daughter particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  /// get the second daughter
  const LHCb::Particle* second = LoKi::Child::child( p , secondIndex() ) ;
  if ( 0 == second ) 
  {
    Error ("Invalid second daughter particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  // tool is valid?
  if ( !tool() ) { loadTool () ; }
  
  // evaluate the result
  return chi2 ( first , second ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCAChi2::fillStream ( std::ostream& s ) const
{ return s << "DOCACHI2(" << firstIndex() << "," << secondIndex() << ",'"
           << toolName () << "')" ; }
// ============================================================================



// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAMax::DOCAMax
( const IDistanceCalculator* dc )
  : LoKi::Particles::DOCA( 1 , 1 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAMax::DOCAMax
( const LoKi::Interface<IDistanceCalculator>& dc )
  : LoKi::Particles::DOCA( 1 , 1 , dc ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCAMax::DOCAMax
( const std::string& nick )
  : LoKi::Particles::DOCA( 1 , 1 , nick ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCAMax::result_type
LoKi::Particles::DOCAMax::operator()
  ( LoKi::Particles::DOCAMax::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  const SmartRefVector<LHCb::Particle>& daugs = p->daughters() ;
  if ( daugs.empty() ) 
  {
    Error ("Empty list of daughetrs, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  // check the tool 
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result
  return docamax ( daugs ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCAMax::fillStream ( std::ostream& s ) const
{ return s << "DOCAMAX(" << "'" << toolName () << "')" ; }
// ============================================================================

// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2Max::DOCAChi2Max
( const IDistanceCalculator* dc )
  : LoKi::Particles::DOCAMax( dc ) 
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2Max::DOCAChi2Max
( const LoKi::Interface<IDistanceCalculator>& dc )
  : LoKi::Particles::DOCAMax( dc ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCAChi2Max::DOCAChi2Max
( const std::string& nick )
  : LoKi::Particles::DOCAMax( nick ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCAChi2Max::result_type
LoKi::Particles::DOCAChi2Max::operator()
  ( LoKi::Particles::DOCAChi2Max::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  const SmartRefVector<LHCb::Particle>& daugs = p->daughters() ;
  if ( daugs.empty() ) 
  {
    Error ("Empty list of daughetrs, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  // check the tool 
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result
  return docachi2max ( daugs ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCAChi2Max::fillStream ( std::ostream& s ) const
{ return s << "DOCACHI2MAX(" << "'" << toolName () << "')" ; }


// ============================================================================
// The END 
// ============================================================================
