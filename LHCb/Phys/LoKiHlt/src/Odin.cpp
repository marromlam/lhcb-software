// $Id: Odin.cpp,v 1.4 2010-05-17 16:01:39 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
#include <numeric>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/EvtNum.h"
#include "LoKi/Odin.h"
#include "LoKi/ToCpp.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for classes from namesapce LoKi::Odin
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @date 2008-09-17 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// the constructor from the start/stop times
// ============================================================================
LoKi::Odin::InTime::InTime 
( const Gaudi::Time& start , 
  const Gaudi::Time& stop  ) 
  : LoKi::AuxFunBase ( std::tie ( start , stop ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_start ( start ) 
  , m_stop  ( stop  ) 
{}
// ============================================================================
// the constructor from the start&span times
// ============================================================================
LoKi::Odin::InTime::InTime 
( const Gaudi::Time&     start , 
  const Gaudi::TimeSpan& span  ) 
  : LoKi::AuxFunBase ( std::tie ( start , span ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_start ( start        ) 
  , m_stop  ( start + span ) 
{}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::InTime::result_type 
LoKi::Odin::InTime::operator() 
  ( LoKi::Odin::InTime::argument o ) const 
{
  const Gaudi::Time time = o -> eventTime () ;
  //
  return m_start <= time && time < m_stop ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::InTime::fillStream ( std::ostream& s ) const 
{
  return s << "ODIN_TIME(" 
           << "Gaudi.Time(" << m_start . ns () << ","
           << "Gaudi.Time(" << m_stop  . ns () << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::Odin::RunNumber::RunNumber
( const LoKi::Odin::RunNumber::run_type run ) 
  : LoKi::AuxFunBase ( std::tie ( run ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One     ) 
  , m_begin ( run     ) 
  , m_end   ( run + 1 ) 
  , m_runs  (         ) 
{}
// ============================================================================
// constructor from the run range 
// ============================================================================
LoKi::Odin::RunNumber::RunNumber
( const LoKi::Odin::RunNumber::run_type begin ,
  const LoKi::Odin::RunNumber::run_type end   ) 
  : LoKi::AuxFunBase ( std::tie ( begin , end  ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( Range ) 
  , m_begin ( begin ) 
  , m_end   ( end   ) 
  , m_runs  (       ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::Odin::RunNumber::RunNumber
( const LoKi::Odin::RunNumber::run_list& runs )
  : LoKi::AuxFunBase ( std::tie ( runs ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_begin ( 0    ) 
  , m_end   ( 0    ) 
  , m_runs  ( runs ) 
{
  std::sort ( m_runs.begin() , m_runs.end() ) ;  
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Odin::RunNumber::~RunNumber(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Odin::RunNumber*
LoKi::Odin::RunNumber::clone() const 
{ return new LoKi::Odin::RunNumber ( *this ) ; }
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::RunNumber::result_type 
LoKi::Odin::RunNumber::operator() 
  ( LoKi::Odin::RunNumber::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : ;
  case Range :
    return m_begin <= o->runNumber() && o->runNumber() < m_end ; // RETURN
  case List:
    return std::binary_search
      ( m_runs.begin() , m_runs.end() , o->runNumber() ) ;       // RETURN
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::RunNumber::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_RUNNUMBER(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_begin << ")" ;                            // RETURN 
  case Range :
    return s << m_begin << "," << m_end << ")" ;            // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_runs ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================

// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::Odin::BXId::BXId
( const unsigned int bx   ) 
  : LoKi::AuxFunBase ( std::tie ( bx ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One ) 
  , m_bx    ( bx ) 
  , m_begin ( 0   ) 
  , m_end   ( 0   ) 
  , m_bxs   (     ) 
{}
// ============================================================================
// constructor from the run range 
// ============================================================================
LoKi::Odin::BXId::BXId
( const unsigned int begin , 
  const unsigned int end   ) 
  : LoKi::AuxFunBase ( std::tie ( begin , end ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( Range ) 
  , m_bx    ( 0     ) 
  , m_begin ( begin ) 
  , m_end   ( end   ) 
  , m_bxs   (       ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::Odin::BXId::BXId
( const std::vector<unsigned int>& bxs ) 
  : LoKi::AuxFunBase ( std::tie ( bxs ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_bx    ( 0    ) 
  , m_begin ( 0    ) 
  , m_end   ( 0    ) 
  , m_bxs   ( bxs  ) 
{
  std::sort ( m_bxs.begin() , m_bxs.end() ) ;
}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::BXId::result_type 
LoKi::Odin::BXId::operator() 
  ( LoKi::Odin::BXId::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : 
    return m_bx == o->bunchId() ;                             // RETURN 
  case Range :
    return m_begin <= o->bunchId() && o->bunchId() < m_end ;  // RETURN
  case List:
    return std::binary_search 
      ( m_bxs.begin() , m_bxs.end() , o->bunchId() ) ;        // RETURN
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::BXId::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_BXID(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_bx << ")" ;                              // RETURN 
  case Range :
    return s << m_begin << "," << m_end << ")" ;            // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_bxs ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the run number 
// ============================================================================
LoKi::Odin::Tck::Tck
( const unsigned int bx   ) 
  : LoKi::AuxFunBase ( std::tie ( bx ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One ) 
  , m_bx    ( bx ) 
  , m_bxs   (     ) 
{}
// ============================================================================
// constructor from the run list 
// ============================================================================
LoKi::Odin::Tck::Tck
( const std::vector<unsigned int>& bxs ) 
  : LoKi::AuxFunBase ( std::tie ( bxs ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_bx    ( 0    ) 
  , m_bxs   ( bxs  ) 
{
  std::sort ( m_bxs.begin() , m_bxs.end() ) ; 
}
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::Tck::result_type 
LoKi::Odin::Tck::operator() 
  ( LoKi::Odin::Tck::argument o ) const 
{
  // 
  switch ( m_flag ) 
  {
  case One   : 
    return m_bx == o->triggerConfigurationKey() ;             // RETURN 
  case List:
    return std::binary_search 
      ( m_bxs.begin() , m_bxs.end() , o->triggerConfigurationKey() ) ; // RETURN
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::Tck::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_TCKEYS(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_bx << ")" ;                              // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_bxs ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================





// ============================================================================
// constructor from the event number 
// ============================================================================
LoKi::Odin::EvtNumber::EvtNumber
( const LoKi::Odin::EvtNumber::event_type evt ) 
  : LoKi::AuxFunBase ( std::tie ( evt ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( One     ) 
  , m_begin ( evt     ) 
  , m_end   ( evt + 1 ) 
  , m_evts  (         ) 
{}
// ============================================================================
// constructor from event range 
// ============================================================================
LoKi::Odin::EvtNumber::EvtNumber
( const LoKi::Odin::EvtNumber::event_type begin ,  
  const LoKi::Odin::EvtNumber::event_type end   ) 
  : LoKi::AuxFunBase ( std::tie ( begin , end ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( Range ) 
  , m_begin ( begin ) 
  , m_end   ( end   ) 
  , m_evts  (       ) 
{}
// ============================================================================
// constructor from event list 
// ============================================================================
LoKi::Odin::EvtNumber::EvtNumber
( const LoKi::Odin::EvtNumber::event_list& evts )
  : LoKi::AuxFunBase ( std::tie ( evts ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag  ( List ) 
  , m_begin ( 0    ) 
  , m_end   ( 0    ) 
  , m_evts  ( evts ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Odin::EvtNumber::~EvtNumber(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Odin::EvtNumber*
LoKi::Odin::EvtNumber::clone() const 
{ return new LoKi::Odin::EvtNumber(*this) ; }
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::EvtNumber::result_type 
LoKi::Odin::EvtNumber::operator() 
  ( LoKi::Odin::EvtNumber::argument o ) const 
{
  // 
  event_type evt ( o->eventNumber() ) ;
  //
  switch ( m_flag ) 
  {
  case One   : ;
  case Range :
    //
    return m_begin <= evt && evt < m_end ; // RETURN
    //
  case List:
    //
    return m_evts.contains ( evt )      ; // RETURN 
    //
  default:
    //
    break ;
  }
  //
  return false ;
  //
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::EvtNumber::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_EVTNUMBER(" ;
  //
  switch ( m_flag ) 
  {
  case One :
    return s << m_begin << ")" ;                            // RETURN 
  case Range :
    return s << m_begin << "," << m_end << ")" ;            // RETURN 
  case List :
    return s << Gaudi::Utils::toString ( m_evts ) << ")" ;  // RETURN
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the run/event number 
// ============================================================================
LoKi::Odin::RunEvtNumber::RunEvtNumber 
( const LoKi::Odin::RunEvtNumber::run_type run , 
  const LoKi::Odin::RunEvtNumber::evt_type evt ) 
  : LoKi::AuxFunBase ( std::tie ( run , evt  ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag    ( One ) 
  , m_begin   ( run , evt           ) 
  , m_end     ( run , evt.evt() + 1 ) 
  , m_runevts (     ) 
{}
// ============================================================================
// constructor from the run/event number 
// ============================================================================
LoKi::Odin::RunEvtNumber::RunEvtNumber 
( const LoKi::Odin::RunEvtNumber::runevt_type& runevt )  
  : LoKi::AuxFunBase ( std::tie ( runevt  ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag    ( One    ) 
  , m_begin   ( runevt ) 
  , m_end     ( runevt.run() , runevt.evt() + 1 ) 
  , m_runevts (        ) 
{}
// ============================================================================
// constructor from the run/event range
// ============================================================================
LoKi::Odin::RunEvtNumber::RunEvtNumber 
( const LoKi::Odin::RunEvtNumber::runevt_type& begin, 
  const LoKi::Odin::RunEvtNumber::runevt_type& end  ) 
  : LoKi::AuxFunBase ( std::tie ( begin , end   ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag    ( Range ) 
  , m_begin   ( begin ) 
  , m_end     ( end   ) 
  , m_runevts (       )  
{}
// ============================================================================
// constructor from the run/event list
// ============================================================================
LoKi::Odin::RunEvtNumber::RunEvtNumber 
( const LoKi::Odin::RunEvtNumber::runevt_list&  runevts ) 
  : LoKi::AuxFunBase ( std::tie ( runevts ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_flag    ( List ) 
  , m_begin   () 
  , m_end     () 
  , m_runevts ( runevts )  
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Odin::RunEvtNumber::~RunEvtNumber(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Odin::RunEvtNumber*
LoKi::Odin::RunEvtNumber::clone() const
{ return new LoKi::Odin::RunEvtNumber ( *this ) ; }
// ============================================================================
// MANDATORY: The only one essential method:
// ============================================================================
LoKi::Odin::RunEvtNumber::result_type 
LoKi::Odin::RunEvtNumber::operator() 
  ( LoKi::Odin::RunEvtNumber::argument o ) const 
{
  // 
  runevt_type runevt ( o->runNumber() , o->eventNumber() );
  //
  switch ( m_flag ) 
  {
  case One   : ;
  case Range :
    //
    return m_begin <= runevt && runevt < m_end ;                      // RETURN
    //
  case List:
    //
    return m_runevts.contains ( runevt ) ;                            // RETURN 
    //
  default:
    break ;
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::RunEvtNumber::fillStream ( std::ostream& s ) const 
{
  s << "ODIN_RUNEVT ( " ;
  //
  switch ( m_flag ) 
  {
  case One :
    //
    return s << m_begin.run() << " , " << m_begin.event() << " ) " ; // RETURN 
    //
  case Range :
    //
    return s << 
      Gaudi::Utils::toString ( m_begin ) << " , " << 
      Gaudi::Utils::toString ( m_end   ) << " ) " ;                   // RETURN 
    //
  case List :
    //
    return Gaudi::Utils::toStream ( m_runevts , s ) << " ) " ;       // RETURN
    //
  default: break ;
  }
  return s << ")" ;
}
// ============================================================================
   


// ============================================================================
// constructor from routing bits 
// ============================================================================
LoKi::Odin::RoutingBits::RoutingBits
( const LoKi::HLT::RoutingBits& bits ) 
  : LoKi::AuxFunBase ( std::tie ( bits ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate() 
  , m_bits ( bits ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Odin::RoutingBits::~RoutingBits () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Odin::RoutingBits*
LoKi::Odin::RoutingBits::clone() const 
{ return new LoKi::Odin::RoutingBits ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Odin::RoutingBits::result_type 
LoKi::Odin::RoutingBits::operator() 
  ( LoKi::Odin::RoutingBits::argument /* a */ ) const 
{
  return m_bits() ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Odin::RoutingBits::fillStream ( std::ostream& s ) const 
{ return s << " ODIN_" << m_bits ; }
// ============================================================================


// ============================================================================
// constructor from the modulo 
// ============================================================================
LoKi::Odin::Evt1::Evt1
( const unsigned long long modulo ) 
  : LoKi::AuxFunBase ( std::tie ( modulo ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Function () 
  , m_modulo ( modulo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Odin::Evt1::~Evt1 (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Odin::Evt1*
LoKi::Odin::Evt1::clone() const 
{ return new LoKi::Odin::Evt1 ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Odin::Evt1::result_type
LoKi::Odin::Evt1::operator() 
  ( LoKi::Odin::Evt1::argument a ) const 
{ return a -> eventNumber() % modulo() ; }
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::Evt1::fillStream ( std::ostream& s ) const 
{ return s << " ODIN_EVT1(" << modulo() << ") " ; }
// ============================================================================


// ============================================================================
// constructor from the modulo 
// ============================================================================
LoKi::Odin::Evt2::Evt2
( const unsigned long long modulo ) 
  : LoKi::AuxFunBase ( std::tie ( modulo ) ) 
  , LoKi::Odin::Evt1 ( modulo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Odin::Evt2::~Evt2 (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Odin::Evt2*
LoKi::Odin::Evt2::clone() const 
{ return new LoKi::Odin::Evt2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Odin::Evt2::result_type
LoKi::Odin::Evt2::operator() 
  ( LoKi::Odin::Evt1::argument a ) const 
{
  //
  const unsigned long long r = a -> eventNumber() / modulo() ; 
  //
  return r ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Odin::Evt2::fillStream ( std::ostream& s ) const 
{ return s << " ODIN_EVT2(" << modulo() << ") " ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
