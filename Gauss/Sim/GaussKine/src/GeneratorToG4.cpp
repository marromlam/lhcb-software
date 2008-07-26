// $Id: GeneratorToG4.cpp,v 1.9 2008-07-26 17:29:33 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"

// from GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"

// from LHCb
#include "Kernel/ParticleID.h"
#include "Event/HepMCEvent.h"

// from Geant4
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleTable.hh"

// local
#include "GeneratorToG4.h"

// Little auxiliary class
class compareBarCode {
public:
  bool operator() (HepMC::GenParticle* partA, HepMC::GenParticle* partB) {
    return partA->barcode() < partB->barcode();
  }
};

//-----------------------------------------------------------------------------
// Implementation file for class : GeneratorToG4
//
// 2005-10-06 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GeneratorToG4 );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeneratorToG4::GeneratorToG4( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
{

  declareProperty( "GiGaService",    m_gigaSvcName    = "GiGa" );
  declareProperty( "HepMCEventLocation", 
                   m_location = LHCb::HepMCEventLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
GeneratorToG4::~GeneratorToG4() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GeneratorToG4::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  sc = m_rand.initialize( randSvc() , Rndm::Flat( 0. , 6.282 ) ) ;
  
  debug() << "==> Initialize" << endmsg;

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName, true ); // Create if necessary

  m_pSvc = svc<IParticlePropertySvc>("ParticlePropertySvc");
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GeneratorToG4::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::HepMCEvents* hepEvts = get<LHCb::HepMCEvents> ( m_location );
  
  // Temporary container of particles to store
  std::vector<HepMC::GenParticle*> outParts;
  outParts.clear();
  
  
  if( hepEvts->size() == 0 ) {
    *gigaSvc() << NULL;
  }
  
  // Loop over the events (one for each pile-up)
  for( LHCb::HepMCEvents::const_iterator iEvt = hepEvts->begin(); 
       hepEvts->end() != iEvt; ++iEvt ) {

    // Loop over the vertices in HepMC::GenEvent
    for( HepMC::GenEvent::vertex_const_iterator
           iVtx = (*iEvt)->pGenEvt()->vertices_begin(); 
         iVtx != (*iEvt)->pGenEvt()->vertices_end(); ++iVtx ) {
      
      // Loop over outgoing particles, check for those with status 1,2, 888 or 
      // 889 and store them in a temporary vector (see explanation of code in
      // Gauss User Manual
      for( HepMC::GenVertex::particles_out_const_iterator
             iPart = (*iVtx)->particles_out_const_begin();
           (*iVtx)->particles_out_const_end() != iPart; ++iPart ) {
        
        if( ( (*iPart)->status() == 1 ) ||
            ( (*iPart)->status() == 2 ) ||
            ( (*iPart)->status() == 888 ) ||
            ( (*iPart)->status() == 889 ) ) {
          // Now choose which one to convert to G4, criteria are:
          // -> the particle has no production vertex (then convert all the
          //             daughters)
          if( !((*iPart)->production_vertex()) ) {
            outParts.push_back( *iPart );
          } 
          // -> the particle is a hadron, a lepton, a nucleus or a photon
          //             and has only one mother which is not a hadron,
          //             not a lepton and not a nucleus and not a photon 
          //             i.e. in practice which is a string (the convert 
          //             all the daughters)
          // -> the particle is hadron, a lepton, a nucleus or a photon 
          //             and has several mother (in pratice a quark-gluon
          //             interaction) or no mother, but a production vertex
          //             (that is to say a particle gun or machine background)
          else {
            if( (*iPart)->production_vertex()->particles_in_size() == 1 ) {
              LHCb::ParticleID pidMother( (*(*iPart)->production_vertex()
                                     ->particles_in_const_begin())->pdg_id() );
              LHCb::ParticleID pid( (*iPart)->pdg_id() );
              if( ( ( ( !pidMother.isHadron() ) &&
                    ( !pidMother.isLepton() ) &&
                    ( !pidMother.isNucleus() ) &&
                    ( pidMother.abspid() != 22 ) ) ||
                    ( (*(*iPart)->production_vertex()->particles_in_const_begin())
                      ->status() == 3 ) ) && 
                  ( ( pid.isHadron() ) ||
                    ( pid.isLepton() ) ||
                    ( pid.isNucleus() ) ||
                    ( pid.abspid() == 22 ) ) ) {
                outParts.push_back( *iPart );
              }
            } else { 
              LHCb::ParticleID pid( (*iPart)->pdg_id() );
              if( ( pid.isHadron( ) ) ||
                  ( pid.isLepton( ) ) ||
                  ( pid.isNucleus( ) ) ||
                  ( pid.abspid() == 22 ) ) {
                outParts.push_back( *iPart );
              }
            }
          }
        }    
      } // end of for loop on particles
    }   // end of for loop on vertices
    
    // Sort the vector, so that particles are always given to G4 in the 
    // same order
    std::sort(outParts.begin(), outParts.end(), compareBarCode() );
    
    // The assumption is made here that all particles produced by Pythia
    // have the same PHYSICAL vertex
    G4PrimaryVertex* origVertex = new G4PrimaryVertex
      ( (*(outParts.begin()))->production_vertex()->position().x(),
        (*(outParts.begin()))->production_vertex()->position().y(),
        (*(outParts.begin()))->production_vertex()->position().z(),
        (*(outParts.begin()))->production_vertex()->position().t() );
    
    for( std::vector<HepMC::GenParticle*>::iterator iOut = outParts.begin();
         outParts.end() != iOut; ++iOut ) {
      G4PrimaryParticle* aParticle = makeG4Particle( *iOut, (*iEvt) );
      origVertex->SetPrimary( aParticle );
    }
    
    *gigaSvc() << origVertex;
    outParts.clear();

  } // end loop on events

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GeneratorToG4::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
G4PrimaryParticle* GeneratorToG4::makeG4Particle(HepMC::GenParticle* genPart,
                                                 LHCb::HepMCEvent* theEvent ) {

  //  static const std::string 
  //  ErrMsg1("GiGaCnv::GenPartG4Part: GenParticle* points to NULL!");

  //if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  
  std::vector<HepMC::GenParticle*> outParticles;
  
  G4PrimaryParticle*  g4Particle = 
    new G4PrimaryParticle( genPart->pdg_id() , 
                           genPart->momentum().px() ,
                           genPart->momentum().py() ,
                           genPart->momentum().pz() );
//                            genPart->momentum().pz() ,
//                            genPart->momentum().e()  );  
  //If PDGID > Nucleus, Get Name, find G4 particle definition and reset that of the particle
  LHCb::ParticleID pid( genPart->pdg_id() );
  if( pid.isNucleus() ){

    ParticleProperty* prop = m_pSvc->findByStdHepID(genPart->pdg_id());
    std::string particlename;
    if( prop ) { particlename = prop->particle(); }
    G4ParticleTable* g4Table = G4ParticleTable::GetParticleTable();
    //find G4 particle definition
    G4ParticleDefinition* g4PartCode = g4Table->FindParticle(particlename);
    g4Particle->SetPDGcode(genPart->pdg_id());
    g4Particle->SetG4code(g4PartCode);
    //create Hep3Vector orthogonal to momentum direction, use as polarisation of light
    
    G4double x1 = genPart->momentum().px();
    G4double y1 = genPart->momentum().py();
    G4double z1 = genPart->momentum().pz();

    G4double Phi = m_rand();
    G4double sinPhi = sin(Phi);
    G4double cosPhi = cos(Phi);
    G4double Theta = acos( z1 / sqrt( (x1 * x1) + (y1 * y1) + (z1 * z1) ) );
    G4double sinTheta = sin(Theta);
    G4double cosTheta = cos(Theta);

    G4double sx = cosTheta*cosPhi;
    G4double sy = cosTheta*sinPhi;
    G4double sz = -sinTheta;


    Hep3Vector momDir(genPart->momentum().px(),
                      genPart->momentum().py(),
                      genPart->momentum().pz());
    
    //Hep3Vector polDir(momDir.orthogonal());
    g4Particle->SetPolarization(sx,
				sy,
				sz);
    //g4Particle->Print();
    //std::cout<<"***Polarisation magnitude*** "
    //     <<sqrt( (sx * sx) + (sy * sy) + (sz * sz) )
    //     <<std::endl;
  }
  g4Particle -> SetMass( genPart -> momentum().m() ) ;

  
  // create a new User information to contain the link to the HepMC
  // particle and signal information for particles with 889 status
  bool isSignalParticle = ( 889 == (genPart->status()) );
  GiGaPrimaryParticleInformation* gInfo = 
    new GiGaPrimaryParticleInformation( isSignalParticle, genPart->barcode(),
                                        theEvent );
  g4Particle -> SetUserInformation( gInfo ) ;
  
  if( genPart->end_vertex() ) {
    // assign decay time
    Gaudi::LorentzVector theLorentzV( genPart->end_vertex()->position().x() -
                                      genPart->production_vertex()->position().x(),
                                      genPart->end_vertex()->position().y() -
                                      genPart->production_vertex()->position().y(),
                                      genPart->end_vertex()->position().z() -
                                      genPart->production_vertex()->position().z(),
                                      genPart->end_vertex()->position().t() -
                                      genPart->production_vertex()->position().t());
    // switch units for time to distance (mm)
    theLorentzV.SetE( theLorentzV.t() * Gaudi::Units::c_light ) ;

    Gaudi::LorentzVector mom( genPart->momentum() ) ;
    ROOT::Math::Boost theBoost( -mom.BoostToCM() ) ;

    // switch again to time from distance (ns)
    Gaudi::LorentzVector newPosition = theBoost( theLorentzV ) ;
    g4Particle->SetProperTime(newPosition.T()/Gaudi::Units::c_light);
    
    // Better to use print here instead of verbose because
//     verbose() << "assigning time " 
//               << (theLorentzV.boost(-theBoost)).t()/c_light
//               << " to " <<  genPart->pdg_id() << endmsg;
    
    if( ( genPart->end_vertex() )->particles_out_size() == 1 ) {
      HepMC::GenVertex::particles_out_const_iterator dPart =
        (genPart->end_vertex())->particles_out_const_begin();
      if( abs( (*dPart)->pdg_id() ) == abs( genPart->pdg_id() ) ) {
        if( (*dPart)->pdg_id() == -(genPart->pdg_id()) ) {
          // particle has oscillated
          // put this info in GiGaPrimaryGenPartInformation
          if ( 0 == g4Particle->GetUserInformation() ) {
            GiGaPrimaryParticleInformation* gInfo =
              new GiGaPrimaryParticleInformation( true ) ;
            g4Particle -> SetUserInformation( gInfo ) ;
          }
          else {
            // update the existing one
            GiGaPrimaryParticleInformation* theInfo =
              gigaPrimaryParticleInformation( g4Particle->GetUserInformation( ) ); 
            if( theInfo ) theInfo->setHasOscillated( true );
          }
        }
        // skip this daughter particle.
        // Since the particle has oscillated it has a end_vertex()
        // and end_vertex() for the daughter is not NULL !
        genPart = ( *dPart );
      }
    }              
    
    // if particle has daughters, carry on with the conversion
    for( HepMC::GenVertex::particles_out_const_iterator outPart = 
           (genPart->end_vertex())->particles_out_const_begin();
         outPart != (genPart->end_vertex())->particles_out_const_end(); ++outPart ) {
      outParticles.push_back(*outPart);
    }

    // sort the vector, so we always put them in the same order into G4
    std::sort(outParticles.begin(), outParticles.end(), compareBarCode());
    
    for( std::vector<HepMC::GenParticle*>::iterator iOutPart = outParticles.begin();
         outParticles.end() != iOutPart; ++iOutPart ) {
      g4Particle->SetDaughter( makeG4Particle(*iOutPart, theEvent) );
    }
  }
  
  return g4Particle;  
};

