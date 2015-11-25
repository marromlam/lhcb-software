// $Id: $
#ifndef COMPONENT_MUONKALMANMATCHTOOL_H 
#define COMPONENT_MUONKALMANMATCHTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonID/IMuonMatchTool.h"            // Interface
#include "Kernel/LHCbID.h"
#include "Kernel/ParticleID.h"

class DeMuonDetector;
class TLinearFitter;
class ITrackExtrapolator;
class ITrackFitter;

namespace LHCb {
  class State;
}

/** @class MuonKalmanMatchTool MuonKalmanMatchTool.h component/MuonKalmanMatchTool.h
 *  
 *  muon match tool based on Kalman filter through Muon stations
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-11
 */
class MuonKalmanMatchTool : public GaudiTool, virtual public IMuonMatchTool {
public: 
  /// Standard constructor
  MuonKalmanMatchTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~MuonKalmanMatchTool() {} ///< Destructor
  virtual StatusCode initialize();

  virtual StatusCode run(const LHCb::Track* track, 
                         std::vector<TrackMuMatch>* bestMatches,
                         std::vector<TrackMuMatch>* spareMatches=NULL);
  virtual void getListofCommonMuonHits(CommonConstMuonHits& matchedMuonHits, int station=-1, bool onlybest=true);
  virtual void getListofMuonTiles(std::vector<LHCb::MuonTileID>& matchedTiles, int station=-1, bool onlybest=true);
  virtual double getChisquare(int& ndof);
  virtual MuonMatchType getMatch(int station);
  virtual double getMatchSigma(int station);
  virtual double muonMatchPropertyD(const char* propertyName, int station=-1);
  virtual int muonMatchPropertyI(const char* propertyName, int  station=-1);

private:
  DeMuonDetector*  m_mudet;
  ITrackExtrapolator* m_extrapolator;
  ITrackFitter* m_trackFitter;
  
  double matchStateToMuonHit(const CommonMuonHit* hit, LHCb::State &StState, double& sigmax, double& sigmay );
  void muOutlierRemoval(LHCb::Track * mutrack, LHCb::LHCbID id);

  LHCb::ParticleID theMuon;
  const LHCb::Track* m_lasttrack;
  std::vector< TrackMuMatch > * m_lastMatchesBest;
  std::vector< TrackMuMatch > * m_lastMatchesSpare;

  int m_missedStations;

  // vectors for the algorithm
  std::vector< LHCb::LHCbID > m_bestMatchedTile;
  std::vector< double > m_bestPM;


  // output
  std::vector<bool> m_hitOnStation;
  CommonConstMuonHits m_matchedHits;
  CommonConstMuonHits m_bestmatchedHits;
  double m_muchi2;
  int m_mudof;
  double m_mymuchi2;
  int m_mymudof; 

  // options
  bool m_OnlyCrossedHits;
  double m_maxDeltaCut;
  double m_maxDeltaCutStation;
  double m_outlierCut;
  int m_maxMatches;

};
#endif // COMPONENT_MUONKALMANMATCHTOOL_H
