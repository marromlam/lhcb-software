// $Id: CaloTrackHcalEval.h,v 1.1.1.1 2003-03-13 18:52:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/11/17 17:09:27  ibelyaev
//  new set of options and tools
//
// Revision 1.1.1.1  2002/11/13 20:46:39  ibelyaev
// new package 
// 
// ============================================================================
#ifndef CALOTRACKHcalEval_H 
#define CALOTRACKHcalEval_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IIncidentListener.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackIdEval.h"
// CaloKernel 
#include "CaloKernel/CaloTool.h"
// 
class TrStoredTrack    ;
class IDataProviderSvc ;
class IIncidentSvc     ;

/** @class CaloTrackHcalEval CaloTrackHcalEval.h
 *  
 *  The Tool which evaluates the energy deposition 
 *  in Hcal for given track
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
class CaloTrackHcalEval :
  public virtual ICaloTrackIdEval  , 
  public virtual IIncidentListener ,
  public          CaloTool        
{
  // friend factory for instantiation 
  friend class ToolFactory<CaloTrackHcalEval>;
public:
  
  /** The main processing method 
   *  @see ICaloTrackIdEval
   *  It evaluates the Hcal energy deposition for the given track 
   *  @param  track  pointer to thobject to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process    
  ( const TrStoredTrack* track , 
    double&              value ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloTrackIdEval
   *  It evaluates the Hcal energy deposition for the given track 
   *  @param  track  pointer to thobject to be processed
   *  @param  the value of the estimator
   */  
  virtual double     operator() 
    ( const TrStoredTrack* track ) const ;

  /** standard initialization method 
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode    initialize ();

  /** standard finalization method 
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
  /** handle the incident 
   *  (reset the local cached values at the end of the event) 
   *  @see Incident 
   *  @see IIncidentListener 
   *  @see IIncidentSvc 
   *  @param incident incident to be handled 
   */
  virtual void handle( const Incident& incident ) ;
   
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service xor tool)
   */
  CaloTrackHcalEval
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloTrackHcalEval(){}; ///< Destructor
  
private:
  
  /// the default constructor is private
  CaloTrackHcalEval();
  /// the copy constructor is private 
  CaloTrackHcalEval           ( const CaloTrackHcalEval& );
  /// the assignement is private 
  CaloTrackHcalEval& operator=( const CaloTrackHcalEval& );

private:
  
  // bad value 
  double              m_bad      ;
  
  // address of container of input digits
  std::string         m_input    ;  

  // event service 
  IDataProviderSvc*   m_eventSvc ;

  // incident  service 
  IIncidentSvc*       m_incSvc   ;
  
  // pointer to digits of current event 
  mutable CaloDigits* m_digits   ;
};

// ============================================================================
// The END
// ============================================================================
#endif // CALOTRACKHcalEval_H
// ============================================================================
