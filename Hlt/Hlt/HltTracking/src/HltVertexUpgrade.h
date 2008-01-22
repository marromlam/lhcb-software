// $Id: HltVertexUpgrade.h,v 1.4 2008-01-22 10:04:25 hernando Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADE_H 
#define HLTTRACKING_HLTTRACKUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltUtils.h"
#include "HltBase/EDictionary.h"
#include "HltTrackUpgradeTool.h"

/** @class HltVertexUpgrade HltVertexUpgrade.h
 *  
 *  functionality:
 *  Options:
 *        RecoName: name of the reconstruction to be done,
 *           valid names: "Velo","VeloTT","Forward","MuonForward"
 *        OrderByPt: true/false order by Pt output tracks in HltDataStore
 *
 *  Note:

 *  @author Jose Angel Hernando Morata
 *  @date   2006-08-28
 */
class HltVertexUpgrade : public HltAlgorithm {
public: 
  /// Standard constructor
  HltVertexUpgrade( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexUpgrade( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode setReco();
  
  void saveConfiguration();

  size_t upgrade(LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
protected:

  std::vector<LHCb::Track*> m_tracks1;
  std::vector<LHCb::Track*> m_tracks2;

  std::string m_recoName;
  bool m_transferExtraInfo;

protected:

  std::string m_TESOutputVerticesName;

  HltTrackUpgradeTool* m_tool;

  Hlt::VertexCreator _makeVertex;

  std::string m_inputName;
  Hlt::VertexSelection* m_inputVertices;
  Hlt::VertexSelection* m_outputVertices;
  

};
#endif // HLTTRACKING_H
