// $Id: CondDBTestAlgorithm.h,v 1.5 2005-10-07 15:40:20 marcocle Exp $
#ifndef CONDDBTESTALGORITHM_H 
#define CONDDBTESTALGORITHM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class DataObject;
class Condition;
class IUpdateManagerSvc;
class IDetDataSvc;

/** @class CondDBTestAlgorithm CondDBTestAlgorithm.h
 *  
 *  Example of an algorithm retrieving condition data stored in the CondDB.
 *
 *  Pointers to relevant DataObjects are retrieved at every new event.
 *  The retrieved objects are explicitly updated to ensure they are valid.
 *
 *  @author Andrea Valassi 
 *  @date December 2001
 *
 *  @author Marco Clemencic
 *  @date   2005-06-23
 */
class CondDBTestAlgorithm : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CondDBTestAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CondDBTestAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  /// Dump interesting information about a DataObject
  StatusCode i_analyse( DataObject* pObj );

  /// Internal member function used with the UpdateManagerSvc
  StatusCode i_updateCacheLHCb();
  /// Internal member function used with the UpdateManagerSvc
  StatusCode i_updateCacheHcal();
  /// Internal member function used with the UpdateManagerSvc
  StatusCode i_updateCache();
  
  /// Cached pointer to Condition
  Condition *m_LHCb_cond;
  /// Cached pointer to Condition
  Condition *m_Hcal_cond;
  
  /// Pointer tu the Update Manager Service
  IUpdateManagerSvc *m_ums;

  double m_LHCb_temp;
  double m_Hcal_temp;
  double m_avg_temp;

  size_t m_evtCount;

  IDetDataSvc *m_dds;

};
#endif // CONDDBTESTALGORITHM_H
