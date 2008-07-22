// $Id: ForcedBDecayTool.cpp,v 1.2 2008-07-22 13:01:43 cattanem Exp $
#include "ForcedBDecayTool.h"
#include "GaudiKernel/ToolFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ForcedBDecayTool v1.0
//
// 2007-02-06 : Marco Musy (Milano)
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( ForcedBDecayTool );

//=============================================================================
ForcedBDecayTool::ForcedBDecayTool( const std::string& type,
				    const std::string& name,
				    const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<IForcedBDecayTool>(this);
}
ForcedBDecayTool::~ForcedBDecayTool(){}

//=============================================================================
StatusCode ForcedBDecayTool::initialize() { 
  return StatusCode::SUCCESS; 
}

StatusCode ForcedBDecayTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
const MCParticle* ForcedBDecayTool::forcedB() {

  //check what is the B forced to decay
  const MCParticle *BS = 0;
  HepMCEvents* hepVect = get<HepMCEvents>( HepMCEventLocation::Default );

  for( std::vector<HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator 
	    p  = (*q)->pGenEvt()->particles_begin();
	  p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( (*p)->status() != 889 ) continue;
      BS = associatedofHEP(*p);
      if(BS) break; 
    }
  }
  return BS;

}
//============================================================================
MCParticle* ForcedBDecayTool::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> ( MCParticleLocation::Default );

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().mag();
  double moththeta = hepmcp->momentum().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 1.0){
	if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
	  return (*imc);
	}
      }
    }
  }
  return 0;
}
//=============================================================================
