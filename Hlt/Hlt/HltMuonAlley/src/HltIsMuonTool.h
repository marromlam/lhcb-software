// $Id: HltIsMuonTool.h,v 1.1 2008-01-31 14:59:55 hernando Exp $
#ifndef HLTISMUONTOOL_H 
#define HLTISMUONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDet/IMuonPosTool.h"
#include "Event/Track.h"
#include "HltBase/IFunctionTool.h"            // Interface

/** @class HltIsMuonTool HltIsMuonTool.h
 *  
 *
 *  @author Leandro de Paula
 *  @date   2008-01-25
 */
class HltIsMuonTool : public GaudiTool, 
                              virtual public ITrackFunctionTool {
public: 
  /// Standard constructor
  HltIsMuonTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~HltIsMuonTool( ); ///< Destructor

  StatusCode initialize();
  
  double function(const LHCb::Track& track);
  bool isInFOI(LHCb::Track* track, LHCb::MuonTileID tile);

protected:
  IMuonPosTool*      m_iPosTool;

private:
// FOI parameters
  std::vector<double> m_p1x;
  std::vector<double> m_p2x;
  std::vector<double> m_p3x;
  std::vector<double> m_p1y;
  std::vector<double> m_p2y;
  std::vector<double> m_p3y;

};
#endif // HLTISMUONTOOL_H
