// $Id: $
#ifndef PARTICLEFLOW4JETS_H 
#define PARTICLEFLOW4JETS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/GenericVectorTypes.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/IHitExpectation.h"
#include "Kernel/IParticle2State.h"
// Event
#include "Event/State.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Kernel/IProtoParticleFilter.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiAlg/ITupleTool.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Relations/RelationWeighted1D.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Kernel/Particle2Vertex.h"


#include "Event/TrackUnitsConverters.h"
#include "LHCbMath/MatrixUtils.h"

#include "Kernel/PFCaloCluster.h"
#include "Kernel/PFParticle.h"

#include "TF1.h"

/** @class ParticleFlow ParticleFlow.h
 *  
 *  Algorithm for ParticleFlow. 
 *
 *  @author Victor Coco
 *  @date   2011-05-10
 */
class TF1;

class ParticleFlow : public GaudiTupleAlg {
public: 

  /// Standard constructor
  ParticleFlow( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleFlow( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum BannedCaloCluster { BadTrack = -1 , InfMom = 0 , TrackMatch ,TrackMatchTT ,TrackMatchHCAL ,TrackMatchHCALTT , Bremsstrahlung ,
                           Photon  , CnvPhoton  , ResolvedPi0 , MergedPi0 , ChargedHadron , BadPhotonMatchingT , BadPhoton } ;

protected:

private:
  
  StatusCode loadDatas() ;

  StatusCode preprocessDatas(LHCb::Particles& PFParticles , Particle2Vertex::WTable& table ) ;

  int tagTrack( const LHCb::Track* track );

  void relate2Vertex(const LHCb::Particle* p , Particle2Vertex::WTable& table);
  
  int numberOfSaturatedCells( const LHCb::CaloCluster* cluster , const DeCalorimeter* calo );
  
  StatusCode updatePFCaloClusters( LHCb::PFParticle& theParticle );

  StatusCode treatProtoParticles( std::string pptype , LHCb::Particles& PFParticles , Particle2Vertex::WTable& table );

  StatusCode treatPi0s( int pptype , LHCb::Particles& PFParticles);

  StatusCode treatPhotons(  LHCb::Particles& PFParticles);

  StatusCode treatNeutralHadrons( LHCb::Particles& PFParticles , LHCb::CaloHypos& PFCaloHypos, LHCb::ProtoParticles& PFProtoParticles );

  StatusCode treatNeutralRecovery( LHCb::Particles& PFParticles );  

  StatusCode checkClustersForNeutralRecovery( LHCb::PFParticle& theParticle , 
                                              LHCb::Calo2Track::IClusTrTable2D::InverseType::Range ecalAssoc, 
                                              LHCb::Calo2Track::IClusTrTable2D::InverseType::Range hcalAssoc );

  Gaudi::Vector6 expectedEnergy(const LHCb::PFParticle* p );

  StatusCode createHiddenNeutral( std::vector < LHCb::PFCaloCluster* > clusters, std::vector < const LHCb::PFParticle*  > particles,  LHCb::Particles& PFParticles);

private:

  
  std::string m_trSelType;                                       ///< The track selector type
 
  ITrackSelector * m_trSel;                                      ///< The track selector tool

  IHitExpectation* m_ttExpectation;                          ///< The TT hit expectation tool

  std::string m_clust2TrLocationECAL;   ///< Location of the Tr 2 ECAL cluster relation table

  LHCb::Calo2Track::IClusTrTable2D* m_tableTrECAL ; ///< The Tr 2 ECAL cluster relation table

  std::string m_clust2TrLocationHCAL;   ///< Location of the Tr 2 HCAL cluster relation table

  LHCb::Calo2Track::IClusTrTable2D* m_tableTrHCAL ; ///< The Tr 2 HCAL cluster relation table

  IParticle2State* m_p2s;                                     ///< The particle to state tool

  IProtoParticleFilter* m_filterGhost ;

  IRelatedPVFinder* m_pvRelator ;

  std::map< unsigned int ,  LHCb::PFCaloCluster* > m_PFCaloClusters;

  std::vector< std::string > m_particleLocations          ;
  std::vector< std::string > m_compositeParticleLocations ;
  std::vector< std::string > m_banCandidatesLocations     ;

  std::map< std::string , std::vector< LHCb::Particles* > > m_particleContainers;

  std::map< std::string , const LHCb::CaloClusters* > m_clusterContainers;

  std::map< std::string , LHCb::ProtoParticles* > m_protoParticles;

  LHCb::RecVertex::Range m_vertices;
  
  std::string m_verticesLocation ;

  std::string m_pf2verticesLocation ;

  std::string m_PFOutputLocation ;

  std::string m_protoPF ;

  std::string m_calohypoPF ;

  double m_Chi2ECALCut;

  double m_Chi2HCAL0Cut;

  double m_Chi2HCAL1Cut;

  double m_Chi2HCAL2Cut;

  double m_Chi2HCAL0CutEValue;

  double m_Chi2HCAL1CutEValue;

  double m_cutInfMomTRVal;

  double m_photonIDMax4ResolvedPi0 ;

  double m_photonIDMin4ResolvedPi0 ;

  double m_photonID4Photon ;

  bool m_catchBremFromElectrons ;

  bool m_useTTHits ; 

  bool m_usePIDInfo;

  bool m_useNNGhost ;

  bool m_useHCAL;

  double m_noTTChi2PerDof ;

  bool m_alsoBanClone ;

  double m_alphaECAL ;
  double m_betaHCAL ;

  std::vector< int > m_electronPPkeys ;

  std::map< int , const LHCb::Track* > m_trackKeyToBan ;


  bool m_banCandidates ;

  double m_minHCALE ;

  double m_minHCALE_NR ;

  double m_minECALE_NR ;

  bool  m_banFromTTrack ;
  
  double  m_Chi2HCALCutTT ;
  
  double m_Chi2ECALCutTT;

  double m_photonID4PhotonTban;

  typedef std::map< std::string , std::pair< const IProtoParticleFilter* , const LHCb::ParticleProperty * > > ProtoMap;
  ProtoMap m_protoMap;

  int m_nSigmaE;
  
  bool m_neutralRecovery;

  bool m_useVelo;

  const DeCalorimeter* m_ecal ;
  const DeCalorimeter* m_hcal ;

  TF1* m_ECALElectron_Data;
  TF1* m_ECALElectron_MC;
  TF1* m_ECALHCALHadron_Data; 
  TF1* m_ECALHCALHadron_MC; 
  TF1* m_ECALHadron_Data;
  TF1* m_ECALHadron_MC;
  TF1* m_HCALHadron_Data;
  TF1* m_HCALHadron_MC; 
  
  double m_MinE;
  double m_maxfractionofE;
  bool m_MC;

  //-----------------------------------------
};


class sortChi2 {
 public:
 inline bool operator() (const LHCb::Particle* obj1 ,
 const LHCb::Particle* obj2) { 
 return obj1->endVertex()->chi2PerDoF() < obj2->endVertex()->chi2PerDoF(); 
 }
};

#endif // PARTICLEFLOW4JETS_H
