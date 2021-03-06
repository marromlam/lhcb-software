#ifndef LSADAPTPVFITTER_H
#define LSADAPTPVFITTER_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Interfaces
#include "IPVFitter.h"
#include "PVUtils.h"

// Track info
#include "Event/Track.h"
#include "Event/RecVertex.h"

// Forward declarations
class ITrackExtrapolator;

class LSAdaptPV3DFitter : public extends<GaudiTool, IPVFitter> {

public:
  // Standard constructor
  LSAdaptPV3DFitter(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  // Destructor
  ~LSAdaptPV3DFitter() override = default;
  // Initialization
  StatusCode initialize() override;
  // Fitting
  StatusCode fitVertex(const Gaudi::XYZPoint& seedPoint,
                       const std::vector<const LHCb::Track*>& tracks,
                       LHCb::RecVertex& vtx,
                       std::vector<const LHCb::Track*>& tracks2remove) const override;
private:
  int    m_minTr;         // Minimum number of tracks to make a vertex
  int    m_Iterations;    // Number of iterations for minimisation
  int    m_minIter;       // iterate at least m_minIter times
  //  double m_maxIP2PV;      // Maximum IP of a track to accept track
  //  double m_maxRDPV;       // Maximum Radial Distance of PV
  double m_maxDeltaZ;     // Fit convergence condition
  double m_minTrackWeight;// Minimum Tukey's weight to accept a track
  double m_TrackErrorScaleFactor;
  double  m_x0MS;         // X0 (tunable) of MS to add for extrapolation of
                          // track parameters to PV
  double m_trackMaxChi2;  // maximum chi2 track to accept track in PV fit
  double m_trackMaxChi2Remove; // Max chi2 tracks to be removed from next PV search
  double m_trackChi;      // sqrt of m_trackMaxChi2
  double m_maxChi2;       // maximal chi2 to accept track
  bool   m_AddMultipleScattering; // add multiple scattering calculation to not fitted tracks
  bool   m_CalculateMultipleScattering; // calculate multiple scattering
  bool   m_UseFittedTracks; // use tracks fitted by kalman fit
  double m_scatCons;      // calculated from m_x0MS and 3 GeV
  double m_scatConsNoMom; // calculated from m_x0MS to be divided by momemntum [MeV]
  double m_myZero;        //myzero=1E-12 small number
  double m_zVtxShift;
  // double m_factor;

  ITrackExtrapolator* m_linExtrapolator;   // Linear extrapolator
  ITrackExtrapolator* m_fullExtrapolator;  // Full extrapolator

  // Add track for PV
  void addTrackForPV(const LHCb::Track* str, PVTracks& pvTracks,
                           const Gaudi::XYZPoint& seed) const;

  double err2d0(const LHCb::Track* track, const Gaudi::XYZPoint& seed) const;
  // Get Tukey's weight
  double getTukeyWeight(double trchi2, int iter) const {
    if (iter<1 ) return 1.;
    auto ctrv = m_trackChi * std::max(m_minIter -  iter, 1);
    auto cT2  = trchi2 / std::pow(ctrv*m_TrackErrorScaleFactor,2);
    return cT2 < 1 ? std::pow( 1-cT2, 2 ) : 0.;
  }
};

#endif // LSADAPTPVFITTER_H
