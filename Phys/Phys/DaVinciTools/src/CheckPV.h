// $Id: CheckPV.h,v 1.3 2004-11-18 09:17:14 pkoppenb Exp $
#ifndef CHECKPV_H 
#define CHECKPV_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CheckPV CheckPV.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-18
 */
class CheckPV : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CheckPV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckPV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IPVLocator* m_PVLocator ;   ///< PVLocator Tool
  int m_minPV ;  ///< minimal number of reconstructed PVs required
  int m_maxPV ;  ///< maximal number of reconstructed PVs required. -1 means no requirement.
  int m_nEvent ; ///< Number of events
  int m_nPV ;    ///< Total number of reconstructed PV

};
#endif // CHECKPV_H
