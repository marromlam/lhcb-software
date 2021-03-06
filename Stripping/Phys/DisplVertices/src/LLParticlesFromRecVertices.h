#ifndef HLTDISPLVERTICES_LLPARTICLESFROMRECVERTICES_H
#define HLTDISPLVERTICES_LLPARTICLESFROMRECVERTICES_H 1

// Include files
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IMatterVeto.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/IParticle2State.h"

#include "VeloDet/DeVelo.h"

#include "LoKi/BasicFunctors.h"

/** @class LLParticlesFromRecVertices LLParticlesFromRecVertices.h
 *  Create particles out of the given RecVertex container.
 *
 *  Tries to use the input particles first (matching them to the Velo tracks
 *  forming the RecVertex), and falls back to creating fixed-PT pions from the
 *  Velo tracks if that fails.
 *  Some cuts are applied (and some quantities stored in the extraInfo)
 *
 *  @author Neal Gauvin
 *  @author Victor Coco
 *  @author Pieter David
 *  @date   2012-03-27
 */
class LLParticlesFromRecVertices : public DaVinciAlgorithm {
public:
  /// Standard constructor
  LLParticlesFromRecVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LLParticlesFromRecVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  // ExtraInfo indices
  static int Rho;
  static int SigmaRho;
  static int SigmaZ;
  static int FractDaughterTracksWithUpstreamHit;
  static int MaxFractEnergyInSingleTrack;
  static int InMatterVeto;
  static int SumPT;

private:
  /// Turn a RecVertex into a Particle
  const LHCb::Particle* RecVertex2Particle( const LHCb::RecVertex* rv );

  // Ad-hoc creation of fixed PT Velo-only pions
  double m_piMass;            ///< pion mass
  double m_pt;                ///< default pt for default pions
  /// Get the momentum for a default fixed-PT pion out of a Velo track
  Gaudi::LorentzVector getDefaultVeloOnlyMomentum( const LHCb::Track* );

  /// Create the best-(NN)PID particle, as in the jet algorithm
  LHCb::Particle* makeBestNNPIDParticle( const LHCb::ProtoParticle* proto );
  typedef std::map< std::string , std::pair< const IProtoParticleFilter* , const LHCb::ParticleProperty * > > ProtoMap;
  ProtoMap m_protoMap;
  IParticle2State* m_p2s;

  // PROPERTIES

  std::vector<std::string> m_RVLocations; ///< Minimal number of tracks required in the "most upstream\"

  unsigned int m_FirstPVNumTracks;        ///< Allow Velo-only daughters
  // More cut values for the most upstream PV (hard-coded)
  double m_FirstPVMaxRho;
  double m_FirstPVMinZ;
  double m_FirstPVMaxZ;
  bool m_requireUpstreamPV;

  bool m_useVeloTracks;                   ///< Maximum track Chi2 for long daughter tracks
  bool m_verticesFromVeloOnly;            ///< Hlt (Velo-only) or Stripping (Rec/Track/Best) vertexing

  double m_chi2NonVeloOnly;               ///< Maximum track Chi2 for long daughter tracks

  std::string m_LLPName;                  ///< LLP ParticleID name
  LHCb::ParticleID m_LLPID;               ///< LLP ParticleID

  double m_LLPMinMass;                    ///< Minimal reconstructed mass of the LLP
  double m_LLPMinSumPT;                   ///< Minimal sum-PT of the LLP

  double m_LLPMinR;                       ///< Minimal LLP RHO distance to the beam line

  int    m_LLPMinNumTracks;               ///< Minimal number of daughter tracks of the LLP

  double m_LLPMaxFractEFromOne;           ///< Maximal allowed fraction of the energy contributed by one daughter

  double m_LLPMaxFractTrWithUpstream;     ///< Maximal allowed fraction of daughters with a hit upstream of the vertex


  bool m_computeMatterVeto;               ///< Add InMatterVeto flag to the created particles
  bool m_applyMatterVeto;                 ///< Reject recvertices inside material region
  const IMatterVeto* m_materialVeto;

  std::string m_chargedProtoLocation;
  LHCb::ProtoParticles* m_chargedProto;

  std::string m_VeloProtoPLocation;
  LHCb::ProtoParticles* m_veloProtoParticles;

  DeVelo* m_velo;

  // debug level flags
  bool m_debug;
  bool m_verbose;

  // functors
  LoKi::BasicFunctors<const LHCb::VertexBase*>::FunctionFromFunction m_RHO;
  LoKi::BasicFunctors<const LHCb::VertexBase*>::FunctionFromFunction m_Z;

  LoKi::BasicFunctors<const LHCb::VertexBase*>::PredicateFromPredicate m_VERTEXCUT;

  LoKi::BasicFunctors<std::vector<const LHCb::RecVertex*> >::FunctionFromFunction m_UPPVZ;

  // Keep vectors in memory instead of allocating every time
  std::vector<LHCb::Particle*> m_tmpParticles;
  std::vector<const LHCb::Particle*> m_selectedDaughters;
  typedef std::pair<const LHCb::Track*,Gaudi::LorentzVector> Track4;
  std::vector<Track4> m_selectedVeloDaughters;

};
#endif // HLTDISPLVERTICES_LLPARTICLESFROMRECVERTICES_H
