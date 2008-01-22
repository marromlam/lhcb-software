// $Id: HltPrepareMuonSeg.h,v 1.3 2008-01-22 09:58:06 hernando Exp $
#ifndef HLTPREPAREMUONSEG_H 
#define HLTPREPAREMUONSEG_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IMuonSeedTool.h"

/** @class HltPrepareMuonSeg HltPrepareMuonSeg.h
 *  
 *
 *  @author Sandra Amato 
 *  @date   2007-12-4
 */
class HltPrepareMuonSeg : public HltAlgorithm {
public: 
  /// Standard constructor
  HltPrepareMuonSeg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltPrepareMuonSeg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
private:
  
  
  IMuonSeedTool* m_prepareMuonSeed;
  
  
  
  std::string m_outputMuonTracksName;   ///< output container name
};
#endif // HLTPREPAREMUONSEG_H
