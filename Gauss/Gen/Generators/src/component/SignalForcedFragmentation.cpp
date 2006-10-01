// $Id: SignalForcedFragmentation.cpp,v 1.10 2006-10-01 22:43:38 robbep Exp $
// Include files

// local
#include "SignalForcedFragmentation.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from LHCb
#include "Kernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenEvent.h"

// from Generators
#include "Generators/IDecayTool.h"
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalForcedFragmentation
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SignalForcedFragmentation>          s_factory ;
const        IToolFactory& SignalForcedFragmentationFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalForcedFragmentation::SignalForcedFragmentation(
  const std::string& type, const std::string& name, const IInterface* parent )
  : Signal( type, name , parent ) , m_signalMass( 0. ) { 
  }

//=============================================================================
// Destructor
//=============================================================================
SignalForcedFragmentation::~SignalForcedFragmentation( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode SignalForcedFragmentation::initialize( ) {
  StatusCode sc = Signal::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  IParticlePropertySvc * ppSvc = 
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;
  ParticleProperty * prop = ppSvc -> findByStdHepID( *m_pids.begin() ) ;
  m_signalMass = prop -> mass() ;

  release( ppSvc ) ;
  
  return sc ;
}

//=============================================================================
// Generate set of events with repeated hadronization
//=============================================================================
bool SignalForcedFragmentation::generate( const unsigned int nPileUp ,
                                          LHCb::HepMCEvents * theEvents ,
                                          LHCb::GenCollisions * theCollisions )
{
  StatusCode sc ;

  // first decay signal particle
  HepMC::GenEvent * theSignalHepMCEvent = new HepMC::GenEvent( ) ;
  HepMC::GenParticle * theSignalAtRest = new HepMC::GenParticle( ) ;
  theSignalAtRest -> 
    set_momentum( HepLorentzVector( 0., 0., 0., m_signalMass ) ) ;

  // Create an origin vertex at (0,0,0,0) for the signal particle at rest
  HepMC::GenVertex * theVertex =  
    new HepMC::GenVertex( HepLorentzVector( 0., 0., 0., 0. ) ) ;
  theSignalHepMCEvent -> add_vertex( theVertex ) ;
  theVertex -> add_particle_out( theSignalAtRest ) ;
  
  bool flip ;
  int theSignalPID = *m_pids.begin() ;

  if ( m_cpMixture ) {
    // decide which flavour to generate : 
    // if flavour < 0.5, b flavour
    // if flavour >= 0.5, bbar flavour
    double flavour = m_flatGenerator() ;

    m_decayTool -> enableFlip() ;
    
    if ( flavour < 0.5 ) 
      theSignalAtRest -> set_pdg_id( +abs( theSignalPID ) ) ;
    else
      theSignalAtRest -> set_pdg_id( -abs( theSignalPID ) ) ;
  } else {
    // generate only one flavour
    m_decayTool -> disableFlip() ;
    theSignalAtRest -> set_pdg_id( theSignalPID ) ;
  }

  m_decayTool -> generateSignalDecay( theSignalAtRest , flip ) ;

  bool result = false ;  

  PIDs signalPid ;
  signalPid.insert( theSignalAtRest -> pdg_id() ) ;

  sc = m_productionTool -> 
    setupForcedFragmentation( theSignalAtRest -> pdg_id() ) ;
  if ( sc.isFailure() ) error() << "Could not force fragmentation" << endmsg ;

  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  // TODO: fix problem when 2 consecutive B events. The 2 B events both have
  // signal in them !

  // Then generate set of pile-up events    
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent ,
                        theGenCollision ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      ParticleVector theParticleList ;
      if ( checkPresence( signalPid , theGenEvent , theParticleList ) ) {
        m_nEventsBeforeCut++ ;
        
        updateCounters( theParticleList , m_nParticlesBeforeCut , 
                        m_nAntiParticlesBeforeCut , false ) ;

        bool passCut = true ;
        if ( 0 != m_cutTool ) 
          passCut = m_cutTool -> applyCut( theParticleList , theGenEvent ,
                                           theGenCollision , m_decayTool , 
                                           m_cpMixture , theSignalAtRest ) ;
        
        if ( passCut && ( ! theParticleList.empty() ) ) {          
          m_nEventsAfterCut++ ;

          updateCounters( theParticleList , m_nParticlesAfterCut , 
                          m_nAntiParticlesAfterCut , true ) ;

          HepMC::GenParticle * theSignal = chooseAndRevert( theParticleList ) ;

          // Now boost signal at rest to frame of signal produced by 
          // production generator
          Gaudi::LorentzVector mom( theSignal -> momentum() ) ;
          ROOT::Math::Boost theBoost( -mom.BoostToCM() ) ;

          // Give signal status
          theSignal -> set_status( LHCb::HepMCEvent::SignalInLabFrame ) ;
          
          sc = boostTree( theSignal , theSignalAtRest , theBoost ) ;
          if ( ! sc.isSuccess() ) Exception( "Cannot boost signal tree" ) ;

          if ( m_cleanEvents ) { 
            sc = isolateSignal( theSignal ) ;
            if ( ! sc.isSuccess() ) Exception( "Cannot isolate signal" ) ;
          }
          
          theGenEvent -> 
            set_signal_process_vertex( theSignal -> end_vertex() ) ;
          theGenCollision -> setIsSignal( true ) ;
          
          // Count signal B and signal Bbar
          if ( theSignal -> pdg_id() > 0 ) ++m_nSig ;
          else ++m_nSigBar ;
          
          // Update counters
          GenCounters::updateHadronCounters( theGenEvent , m_bHadC ,
                                             m_antibHadC , m_cHadC ,
                                             m_anticHadC , m_bbCounter ,
                                             m_ccCounter ) ;
          GenCounters::updateExcitedStatesCounters( theGenEvent ,
                                                    m_bExcitedC ,
                                                    m_cExcitedC ) ;
          
          result = true ;
        } 
      }   
    }
  }

  delete theSignalHepMCEvent ;
  return result ;
}

//=============================================================================
// Boost the Tree theSignal to theVector frame and attach it to theMother
//=============================================================================
StatusCode SignalForcedFragmentation::boostTree( HepMC::GenParticle * 
                                                 theSignal ,
                                                 const HepMC::GenParticle * 
                                                 theSignalAtRest ,
                                                 const ROOT::Math::Boost& 
                                                 theBoost )
  const {
  if ( 0 == theSignalAtRest -> end_vertex() ) return StatusCode::SUCCESS ;
    
  if ( 0 != theSignal -> end_vertex() ) 
    return Error( "The particle has already a decay vertex !" ) ;

  if ( 0 == theSignalAtRest -> production_vertex() )
    return Error( "The particle has no production vertex !" ) ;
  
  // Displacement in original frame
  Gaudi::LorentzVector positionEnd , positionBegin ;
  positionEnd.SetXYZT( theSignalAtRest -> end_vertex() -> position() . x() ,
                       theSignalAtRest -> end_vertex() -> position() . y() ,
                       theSignalAtRest -> end_vertex() -> position() . z() ,
                       theSignalAtRest -> end_vertex() -> position() . t() *
                       CLHEP::c_light ) ;
  
  positionBegin.SetXYZT(theSignalAtRest->production_vertex()->position().x() ,
                        theSignalAtRest->production_vertex()->position().y() ,
                        theSignalAtRest->production_vertex()->position().z() ,
                        theSignalAtRest->production_vertex()->position().t() 
                        * CLHEP::c_light ) ;
  
  Gaudi::LorentzVector position = positionEnd - positionBegin ;
  
  // Displacement in new frame after boost.
  Gaudi::LorentzVector newPosition = theBoost( position ) ;

  // LHCb Units
  Gaudi::LorentzVector newP ;
  newP.SetXYZT( newPosition.X() , newPosition.Y() , newPosition.Z() ,
                newPosition.T() / CLHEP::c_light ) ;
  
  // Add original position
  Gaudi::LorentzVector 
    originalPosition( theSignal -> production_vertex() -> position() ) ;
  newP += originalPosition ;

  // Create new HepMC vertex after boost and add it to the current event    
  HepMC::GenVertex * newVertex = 
    new HepMC::GenVertex( HepLorentzVector(newP.X(), newP.Y() , newP.Z() , 
                                           newP.T()));
  
  theSignal -> parent_event() -> add_vertex( newVertex ) ;
  newVertex -> add_particle_in( theSignal ) ;

  HepMC::GenVertex * sVertex = theSignalAtRest -> end_vertex() ;
  
  HepMC::GenVertex::particles_out_const_iterator child ;
  
  for ( child  = sVertex -> particles_out_const_begin( ) ; 
        child != sVertex -> particles_out_const_end( ) ; ++child ) {
    // Boost all daughter particles and create a new HepMC particle
    // for each daughter
    Gaudi::LorentzVector momentum( (*child) -> momentum() ) ;
    Gaudi::LorentzVector newMomentum = theBoost( momentum ) ;
    int id                           = (*child) -> pdg_id() ;
    int status                       = (*child) -> status() ;
    
    HepMC::GenParticle * newPart =
      new HepMC::GenParticle( HepLorentzVector( newMomentum.Px() , 
                                                newMomentum.Py() , 
                                                newMomentum.Pz() , 
                                                newMomentum.E()   ) , 
                              id , status ) ;
    
    newVertex -> add_particle_out( newPart ) ;
    
    HepMC::GenParticle * theNewSignal             = newPart ;
    const HepMC::GenParticle * theNewSignalAtRest = (*child) ;
    
    // Recursive call to boostTree for each daughter
    boostTree( theNewSignal , theNewSignalAtRest , theBoost ) ;
  }

  return StatusCode::SUCCESS ;
}
