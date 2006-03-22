// $Id: CaloTrackAlg.cpp,v 1.3 2006-03-22 18:25:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.1  2005/05/06 17:48:33  ibelyaev
//  add new base classes: Calo(Histo)TrackAlg
// Revision 1.2 2005/10/14 odescham
// adapt to new track model
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/MsgStream.h" 
#include "Event/Track.h"
#include "CaloUtils/CaloTrackAlg.h"
// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloCluster2TrackAlg
 *  @see CaloCluster2TrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloTrackAlg::CaloTrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : GaudiAlgorithm ( name , svcloc ) 
  , m_use () 
{
  //
  m_use.declareProperties ( this ) ;
} ;
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloTrackAlg::~CaloTrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackAlg::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  {
    std::string msg =  m_use.rejectedTypes() ;
    if ( !msg.empty()    ) 
    {
      msg =  "Rejected Track types      : " + msg ;
      info() << msg << endreq ;
      Warning ( msg , StatusCode::SUCCESS , 0 ) ;
    }
  }
  
  { 
    std::string msg =  m_use.rejectedCategories() ;
    if ( !msg.empty()     ) 
    {
      msg = "Rejected Track categories : " + msg ;
      info() << msg << endreq ;
      Warning ( msg , StatusCode::SUCCESS , 0 ) ;
    }
  }
  
  {
    std::string msg =  m_use.rejectedAlgorithms() ;
    if ( !msg.empty()    ) 
    {
      msg = "Rejected Track algorithms : " + msg ;
      info() << msg << endreq ;
      Warning ( msg , StatusCode::SUCCESS , 0 ) ;
    }
  }
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackAlg::finalize  () { return GaudiAlgorithm::finalize() ; }
// ============================================================================
