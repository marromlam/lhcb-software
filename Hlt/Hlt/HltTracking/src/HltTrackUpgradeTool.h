// $Id: HltTrackUpgradeTool.h,v 1.4 2008-01-22 10:04:25 hernando Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADETOOL_H 
#define HLTTRACKING_HLTTRACKUPGRADETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/HltTypes.h"
#include "HltBase/HltUtils.h"
#include "HltBase/ITrackUpgrade.h"
// NEXT DEV take from here (27/6/07)
// #include "TrackInterfaces/ITracksFromTrack.h"
#include "ITracksFromTrack.h"

/** @class HltTrackUpgradeTool HltTrackUpgradeTool.h
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

static const InterfaceID IID_HltTrackUpgradeTool ( "HltTrackUpgradeTool",1,0 );

class HltTrackUpgradeTool : public GaudiTool, virtual public ITrackUpgrade {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_HltTrackUpgradeTool; }

  /// Standard constructor
  HltTrackUpgradeTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~HltTrackUpgradeTool( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

  StatusCode setReco(const std::string& name);

  void beginExecute();

  // this method is save, it takes care of the memory
  StatusCode upgrade(std::vector<LHCb::Track*>& itracks,
                     std::vector<LHCb::Track*>& otracks);

  // this method is unsave, it does not take care of the memory
  // for every event you should do first beginExecute()!
  StatusCode upgrade(LHCb::Track& seed,
                     std::vector<LHCb::Track*>& track);

protected:



  void recoConfiguration();

  bool isReco(const LHCb::Track& seed);
  
  size_t find(const LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
  void recoDone(LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);

  void addIDs(const LHCb::Track& seed, LHCb::Track& track);

  void printInfo(const std::string& title, 
                 const std::vector<LHCb::Track*>& con);

  void printInfo(const std::string& title, const LHCb::Track& track);

protected:

  std::string m_TESOutput;
  
  std::vector<LHCb::Track*> m_tracks;

  std::string m_recoName;
  int m_recoID;
  bool m_owner;
  int m_trackType;

  bool m_transferExtraInfo;
  bool m_transferIDs;
  bool m_transferAncestor;
  bool m_orderByPt;

protected:

  ISequencerTimerTool* m_timer;
  int m_timerTool;

protected:

  template <class T>
  void recoregister(const std::string& name, const std::string& prop,
                    const T& t) {
    std::string key = name+"/"+prop;m_recoConf.add(key,t);
    info() << " reco ["<<key<<"] = " << t <<endreq;
  }

  zen::dictionary m_recoConf;

  LHCb::Tracks* m_otracks;

  ITracksFromTrack* m_tool;

  Hlt::SortTrackByPt _sortByPt;

};
#endif // HLTTRACKING_H
