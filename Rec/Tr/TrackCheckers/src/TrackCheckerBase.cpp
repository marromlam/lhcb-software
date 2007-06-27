// $Id: TrackCheckerBase.cpp,v 1.1 2007-06-27 15:05:06 mneedham Exp $
// Include files 
#include "TrackCheckerBase.h"
#include "Event/Track.h"

#include "Map.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCheckerBase::TrackCheckerBase( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ),
  m_associator(0,""),
  m_all("ALL"){

  declareProperty( "TracksInContainer",
                   m_tracksInContainer = LHCb::TrackLocation::Default  );
  declareProperty( "Selector",
                   m_selectorName = "MCReconstructible" );
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackMasterExtrapolator" );  
  declareProperty( "LinkerInTable",   m_linkerInTable = ""       );

  declareProperty("SplitByAlgorithm", m_splitByAlgorithm = false);

  declareProperty("GhostClassification",
                  m_ghostToolName = "LongGhostClassification" );

  declareProperty("SelectionCriteria", m_selectionCriteria = "ChargedLong"); 

}

//=============================================================================
// Destructor
//=============================================================================
TrackCheckerBase::~TrackCheckerBase() {}; 

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackCheckerBase::initialize()
{

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Set the path for the linker table Track - MCParticle
  if ( m_linkerInTable == "" ) m_linkerInTable = m_tracksInContainer;

  m_selector = tool<IMCReconstructible>(m_selectorName,
                                        "Selector", this );
  
  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);
  
  // Retrieve the magnetic field and the poca tool
  m_poca = tool<ITrajPoca>("TrajPoca");
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  m_stateCreator = tool<IIdealStateCreator>("IdealStateCreator");
  m_visPrimVertTool = tool<IVisPrimVertTool>("VisPrimVertTool");
  m_ghostClassification = tool<ITrackGhostClassification>(m_ghostToolName,"GhostTool",this); 
  m_decayFinder = tool<IMCDecayFinder>("MCDecayFinder","DecayFinder", this);

  const TrackMaps::RecMap& theMap = TrackMaps::recDescription();
  m_recCat = theMap.find(m_selectionCriteria)->second;

  return StatusCode::SUCCESS;
};

StatusCode TrackCheckerBase::execute(){
  
  if (initializeEvent().isFailure()){
    return Error("Failed to initialize event", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackCheckerBase::initializeEvent(){

  // get the association table 
  m_associator = AsctTool(evtSvc(), m_tracksInContainer);
  m_directTable = m_associator.direct();
  if (!m_directTable)  
    return Error("Failed to find direct table", StatusCode::FAILURE);
  
  m_inverseTable = m_associator.inverse();
  if (!m_inverseTable)  
    return Error("Failed to find inverse table", StatusCode::FAILURE);
  
  return StatusCode::SUCCESS;
} 

TrackCheckerBase::LinkInfo TrackCheckerBase::reconstructed(const LHCb::MCParticle* particle) const{
  
  TrackCheckerBase::LinkInfo info; 
  info.track = 0; // in case it is a ghost
  InverseRange range = m_inverseTable->relations(particle);
  if (!range.empty()) {
    info.track = range.begin()->to();
    info.clone = range.size() - 1u;
    info.purity = range.begin()->weight();
  }
  return info; 
}

const LHCb::MCParticle* TrackCheckerBase::mcTruth(const LHCb::Track* aTrack) const{
  TrackCheckerBase::DirectRange range = m_directTable->relations(aTrack);
  const LHCb::MCParticle* particle = 0;
  if (range.empty() == false) particle = range.begin()->to();
  return particle;
}

bool TrackCheckerBase::bAncestor(const LHCb::MCParticle* mcPart) const{

  // loop back and see if there is a B in the history
  bool fromB = false;
  const LHCb::MCParticle* mother = mcPart->mother();
  while ( mother !=0 && fromB == false) {
    fromB = mother->particleID().hasBottom();
  } // loop
  return fromB;
}


StatusCode TrackCheckerBase::finalize()
{
  return GaudiHistoAlg::finalize();
}


