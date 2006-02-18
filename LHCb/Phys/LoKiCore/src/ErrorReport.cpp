// $Id: ErrorReport.cpp,v 1.2 2006-02-18 18:06:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/24 09:39:41  ibelyaev
// New Import: the core part of restructurized LoKi project
//
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/IReporter.h"
#include "LoKi/Welcome.h"
#include "LoKi/ErrorReport.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::ErrorReport
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
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/// standard (default) constructor
// ============================================================================
LoKi::ErrorReport::ErrorReport()
  : m_errors   (   ) 
  , m_warnings (   ) 
  , m_reporter ( 0 ) 
{
  LoKi::Welcome::instance() ;
};
// ============================================================================

// ============================================================================
/** make a report 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::ErrorReport::report() const  
{ 
  // format printout 
  if( 0 != m_errors.size () || 0 != m_warnings.size () )
    {
      std::cout << "LoKi::ErrorReport\t" ;
      std::cout << "  ALWAYS "  ; 
      std::cout << " Errors/Warnings statistics:  " 
                << m_errors     .size () << "/"
                << m_warnings   .size () << std::endl ; 
      // print errors counter 
      for( Counter::const_iterator error = m_errors.begin() ;
           error != m_errors.end() ; ++error )
        {
          std::cout << "LoKi::ErrorReport\t" ;
          std::cout << "  ALWAYS "  ; 
          std::cout << " #ERRORS    = " << error->second  
                    << " Message='"     << error->first    << "'" << std::endl ;
        }  
      // print warnings
      for( Counter::const_iterator warning = m_warnings.begin() ;
           warning != m_warnings.end() ; ++warning )
        {
          std::cout << "LoKi::ErrorReport\t" ;
          std::cout << "  ALWAYS "  ; 
          std::cout << " #WARNINGS  = " << warning->second 
                    << " Message='"     << warning->first  << "'" << std::endl; 
        }  
    }
  m_errors   .clear () ;
  m_warnings .clear () ;  
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
LoKi::ErrorReport::~ErrorReport() 
{ 
  // reset the reporter 
  m_reporter = 0 ; 
  // make a report
  report();
};
// ============================================================================

// ============================================================================
// ============================================================================
LoKi::ErrorReport& LoKi::ErrorReport::instance()
{
  static LoKi::ErrorReport s_report = ErrorReport() ;
  return s_report ;
};
// ============================================================================

// ============================================================================
/** get the LoKi reporter
 *  @return pointer to currently active LoKi error reporter tool
 */
// ============================================================================
const LoKi::IReporter* LoKi::ErrorReport::reporter    () const
{ return m_reporter ; };
// ============================================================================

// ============================================================================
/** set new active Reporter 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::ErrorReport::setReporter 
( const LoKi::IReporter* reporter ) 
{ m_reporter = reporter ; return StatusCode::SUCCESS ; } ;
// ============================================================================

// ============================================================================
/** Print the error  message, return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ============================================================================
StatusCode LoKi::ErrorReport::Error     
( const std::string&  msg , 
  const StatusCode    st  , 
  const size_t        mx ) const 
{ 
  if ( 0 != m_reporter ) { return m_reporter->Error( msg , st , mx ) ; }
  const size_t ne = ++m_errors[msg] ;
  if ( ne <= mx ) { Print( msg , st , MSG::ERROR ) ; }
  if ( ne == mx ) 
  { Print ( "The ERROR   message '" + msg + 
            "' is suppressed from now" , st , MSG::ERROR ) ; }
  return st ;
};
// ============================================================================

// ============================================================================
/** Print the warning  message, return status code 
 *  @param msg    warning message 
 *  @param st     status code  
 *  @return       status code 
 */
// ============================================================================
StatusCode LoKi::ErrorReport::Warning   
( const std::string&  msg , 
  const StatusCode    st  ,
  const size_t        mx ) const  
{
  if( 0 != m_reporter ) { return m_reporter->Warning( msg , st , mx ) ; }
  const size_t ne = ++m_warnings[msg] ;
  if ( ne <= mx ) { Print( msg , st , MSG::WARNING ) ; }
  if ( ne == mx ) 
  { Print ( "The WARNING message '" + msg + 
            "' is suppressed from now" , st , MSG::WARNING ) ; }
  return st ; 
};
// ============================================================================

// ============================================================================
/** Print the message and return status code 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lev    print level 
 *  @return       status code 
 */
// ============================================================================
StatusCode LoKi::ErrorReport::Print     
( const std::string& msg , 
  const StatusCode   st  ,
  const MSG::Level   lev ) const 
{  
  if( 0 != m_reporter ) { return m_reporter->Print( msg , st , lev ) ; }
  ///
  std::cerr << "LoKi::ErrorReport\t"  ;
  if      ( MSG::ALWAYS  == lev ) { std::cerr << "  ALWAYS "  ; }
  else if ( MSG::FATAL   == lev ) { std::cerr << "   FATAL "  ; }
  else if ( MSG::ERROR   == lev ) { std::cerr << "   ERROR "  ; }
  else if ( MSG::WARNING == lev ) { std::cerr << " WARNING "  ; }
  else if ( MSG::INFO    == lev ) { std::cerr << "    INFO "  ; }
  else if ( MSG::DEBUG   == lev ) { std::cerr << "   DEBUG "  ; }
  else                            { std::cerr << "         "  ; }
  std::cerr << msg ;
  
  if( st.isSuccess() ) { std::cerr << "\t" << "StatusCode=SUCCESS" ; } 
  else                 { std::cerr << "\t" << "StatusCode=FAILURE" ; }
  
  if( st.isFailure() ) { std::cerr << " (" << st.getCode() << ")"; }

  std::cerr << std::endl;
  ///
  return st ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

