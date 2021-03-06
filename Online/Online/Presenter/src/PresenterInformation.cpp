// $Id: PresenterInformation.cpp,v 1.1 2010-08-12 15:43:25 robbep Exp $
// This class
#include "Presenter/PresenterInformation.h" 
#include <boost/date_time/posix_time/posix_time.hpp>

//====================================================================
// Constructor
//====================================================================
PresenterInformation::PresenterInformation( ) : 
  m_presenterMode( pres::Init ) , 
  m_globalHistoryByRun( false ) , 
  m_currentTCK( "default" ),
  m_referenceRun( 1 ),
  m_historyTrendPlots( true ) , 
  m_currentPartition( "" ) , 
  m_rwTimePoint( "" ) , 
  m_rwPastDuration( "" ) ,  
  m_currentTime( 0 ) , 
  m_globalPastDuration( "02:00:00" ) , 
  m_globalStepSize( "00:15:00" ) , 
  m_historyMode( pres::s_timeInterval ) 
{
  if (0 == m_currentTime ) { m_currentTime = new TDatime(); }
  char tmp[40];
  std::time_t myTimeT = std::time( NULL );
  std::tm* myTm = std::localtime( &myTimeT );
  std::strftime( tmp, 40, "%Y%m%dT%H%M%S", myTm );
  m_globalTimePoint = std::string( tmp );
}

//====================================================================
// Destructor
//====================================================================
PresenterInformation::~PresenterInformation( ) { 
  if ( 0 != m_currentTime ) delete m_currentTime ; 
}


//=========================================================================
//  Convert the global time to c time.
//=========================================================================
void PresenterInformation::setTimeC( std::string endTime, std::string duration, bool add ) {
  std::cout << "setTimeC: EndTime " << endTime << " duration " << duration << std::endl;
  boost::posix_time::ptime t;
  if ( "Now" == endTime ) {
    t = boost::posix_time::ptime(boost::posix_time::second_clock::local_time());
  } else {
    if ( endTime.find( "-" ) != std::string::npos ) {
      t = boost::posix_time::time_from_string( endTime );
    } else {
      std::cout << "Convert time string " << endTime << std::endl;
      t = boost::posix_time::from_iso_string( endTime ) ;
      std::cout << "Result : " << t << std::endl;
    }
  }
  std::tm time = boost::posix_time::to_tm( t );
  m_endTimeC   = std::mktime( &time );
  std::cout << "Before duration : " << duration << std::endl;
  if ( duration.find( "-" ) != std::string::npos ) {  // duration is in fact an end time
    t = boost::posix_time::time_from_string( duration );
  } else {
    boost::posix_time::time_duration d = boost::posix_time::duration_from_string( duration ) ;
    std::cout << "After duration d=" << d << std::endl;
    if ( add ) {
      t = t + d;
    } else {
      t = t - d;
    }
  }
  time = boost::posix_time::to_tm( t );
  m_startTimeC = std::mktime( &time );
  if ( m_startTimeC > m_endTimeC ) {
    time_t tmp = m_endTimeC;
    m_endTimeC = m_startTimeC;
    m_startTimeC = tmp;
  }
  std::cout << "   -> start " << std::ctime( &m_startTimeC );
  std::cout << "        end " << std::ctime( &m_endTimeC ) << std::endl;
}
