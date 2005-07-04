// $Id: SVertexABTool.cpp,v 1.2 2005-07-04 15:40:09 pkoppenb Exp $
#include "SVertexABTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SVertexABTool v1.3
//
// DO NOT USE YET: only here for development purpose! (still crashing)
//
// 2005-06-30 : Julien Babel / Marco Musy
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SVertexABTool>          s_factory ;
const        IToolFactory& SVertexABToolFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SVertexABTool::SVertexABTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ISecondaryVertexTool>(this);
  declareProperty( "SkipTrackPurge", m_SkipTrackPurge = false );
}

StatusCode SVertexABTool::initialize() {

  geom = tool<IGeomDispCalculator>("GeomDispCalculator");
  if ( !geom ) {   
    err() << "Unable to Retrieve GeomDispCalculator" << endreq;
    return StatusCode::FAILURE;
  }
  fitter = tool<IVertexFitter>("UnconstVertexFitter");
  if ( !fitter ) {   
    err() << "Unable to Retrieve UnconstVertexFitter" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode SVertexABTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
SVertexABTool::~SVertexABTool(){}

//=============================================================================
std::vector<Vertex> SVertexABTool::buildVertex( const Vertex& RecVert, 
                                                const ParticleVector& vtags ){
  double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl, iperrl, ips, iperrs, probf = -1.0 ; 
  StatusCode sc;
  Vertex vtx;
  std::vector<Vertex> vseeds(0),vtxvect(0);
  std::vector<double> vprobf,vprobf_sorted;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  ParticleVector::const_iterator jp, kp;

  for ( jp = vtags.begin(); jp != vtags.end(); jp++ ) {

    //FIRST seed particle -----------------------------------
    sc = geom->calcImpactPar(**jp, RecVert, ipl, iperrl, ipVec, errMatrix);
    if( sc.isFailure() ) continue;
    if( iperrl > 1.0 ) continue;                                //cut
    if( ipl/iperrl < 2.0 ) continue;                            //cut
    if( ipl/iperrl > 100.0 ) continue;                          //cut

    double lcs=1000.;
    ContainedObject* contObj = (*jp)->origin();
    ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
    if (!proto) continue;
    TrStoredTrack* track = proto->track();
    if((track->measurements()).size() > 5)
      lcs = track->lastChiSq()/((track->measurements()).size()-5);
    if( lcs > 2.0 ) continue;                                   //cut
    //must be long forward 
    if( track->forward() == false ) continue;                   //cut

    //SECOND seed particle -----------------------------------
    for ( kp = (jp+1) ; kp != vtags.end(); kp++ ) {

      sc = geom->calcImpactPar(**kp, RecVert, ips, iperrs, ipVec, errMatrix);
      if( sc.isFailure() ) continue;  
      if( iperrs > 1.0 ) continue;                              //cut 
      if( ips/iperrs < 2.0 ) continue;                          //cut 
      if( ips/iperrs > 100.0 ) continue;                        //cut 

      double lcs=1000.;
      ContainedObject* contObj = (*kp)->origin();
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      TrStoredTrack* track = proto->track();
      if((track->measurements()).size() > 5)
        lcs = track->lastChiSq()/((track->measurements()).size()-5);
      if( lcs > 2.0 ) continue;                                //cut
      //second particle must also be forward
      if( track->forward() == false ) continue;                //cut

      //cut on the z position of the seed
      sc = fitter->fitVertex( **jp, **kp, vtx );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //cut
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //cut

      //if the couple is compatible with a Ks, drop it         //cut
      HepLorentzVector sum = (*jp)->momentum() + (*kp)->momentum();
      if( sum.m()/GeV > 0.490 && sum.m()/GeV < 0.505 
          &&  (*jp)->particleID().abspid() == 211
          &&  (*kp)->particleID().abspid() == 211
          && ((*jp)->particleID().threeCharge())
          * ((*kp)->particleID().threeCharge()) < 0 ) {
        debug() << "This is a Ks candidate! skip."<<endreq;
        //set their energy to 0 so that they're not used afterwards
        HepLorentzVector zero(0.0001,0.0001,0.0001,0.0001);
        (*jp)->setMomentum(zero);
        (*kp)->setMomentum(zero);
        continue;
      }

      //build a likelihood that the combination comes from B ---------
      double probi1, probi2, probp1, probp2, proba, probs, probb;
      // impact parameter
      probi1=ipprob(ipl/iperrl);
      probi2=ipprob(ips/iperrs);
      // pt
      probp1=ptprob((*jp)->pt()/GeV); 
      probp2=ptprob((*kp)->pt()/GeV); 
      // angle
      proba=aprob(((*jp)->momentum().vect()).angle((*kp)->momentum().vect()));
      // total
      probs=probi1*probi2*probp1*probp2*proba;
      probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
      probf = probs/(probs+probb);
      //if( probf < 0.32 ) continue;                             //cut
      if( probf < 0.25 ) continue;                             //cut

      vseeds.push_back(vtx);
      vprobf.push_back(probf);
    }
  }
  debug()<<"vseeds, vprobf size="<<vseeds.size()<<" " <<vprobf.size()<<endreq;
  if(vprobf.size()==0) return vtxvect;

  //find best 2 seeds
  Vertex vfitA(0), vfitB(0);
  vprobf_sorted = vprobf; 
  std::sort(vprobf_sorted.begin(), vprobf_sorted.end()); 
  for(unsigned int i=0; i!=vprobf.size(); ++i) {
    if( vprobf.at(i) == vprobf_sorted.at(0) ) vfitA = vseeds.at(i);
    if(i>0) if( vprobf.at(i) == vprobf_sorted.at(1) ) vfitB = vseeds.at(i);
    debug()<<"seed prob="<<vprobf_sorted.at(i)<<endreq;
  }
  bool Bexist = vprobf.size()>1 ? true:false ;
  
  debug()<<"vfitA ="<<vfitA.position().z()<<endreq;
  debug()<<"vfitB ="<<vfitB.position().z()<<endreq;

  //-----
  //add iteratively other tracks to the seeds ----------------------
  double chiB, zB;
  StatusCode scA, scB;
  ParticleVector pfitA(0), pfitB(0); 
  ParticleVector::const_iterator jpp;
  for(jpp = vtags.begin(); jpp != vtags.end(); jpp++){

    double ip, ipe;
    sc = geom->calcImpactPar(**jpp, RecVert, ip, ipe, ipVec, errMatrix);
    if( !sc ) continue;
    if( ip/ipe < 1.8 ) continue;                                        //cut
    if( ip/ipe > 100 ) continue;                                        //cut
    if( ipe    > 1.5 ) continue;                                        //cut

    //likelihood for the particle to come from B ------
    double probi1=ipprob(ip/ipe);
    double probp1=ptprob((*jpp)->pt()/GeV); 
    double probs=probi1*probp1;
    double probb=(1-probi1)*(1-probp1);
    if( probs/(probs+probb) < 0.2 ) continue;                           //cut

    if( isinVtx(vfitA, *jpp) ) continue;
    if( Bexist ) if( isinVtx(vfitB, *jpp) ) continue;

    //try to put it in A ------------------
    pfitA = toStdVector(vfitA.products());
    debug()<<"xxx0 sizeA="<<pfitA.size()<<" sizeB="<<pfitB.size()<<endreq;
    pfitA.push_back(*jpp);
    debug()<<"xxx1 sizeA="<<pfitA.size()<<" sizeB="<<pfitB.size()<<endreq;
    scA = fitter->fitVertex( pfitA, vfitA );
    double chiA = vfitA.chi2()/vfitA.nDoF(); 
    double zA   = vfitA.position().z()/mm - RVz;

    //try to put it in B ------------------
    if(Bexist) {
      pfitB = toStdVector(vfitB.products());
      pfitB.push_back(*jpp);
      scB  = fitter->fitVertex( pfitB, vfitB );
      chiB = vfitB.chi2()/vfitB.nDoF(); 
      zB   = vfitB.position().z()/mm - RVz;
    }
    debug()<<"xxx2 sizeA="<<pfitA.size()<<" sizeB="<<pfitB.size()<<endreq;

    //decide where to put it for real
    bool putinA=false, putinB=false; 
    if( scA && zA > 1.0 && chiA < 5.0) putinA=true;
    if(Bexist) {
      if( scB && zB > 1.0 && chiB < 5.0) putinB=true;
      if( chiA > chiB ) putinB=false; else putinA=false; 
      //and make action..
      if( ! putinA ) { pfitA.pop_back(); fitter->fitVertex( pfitA, vfitA ); }
      if( ! putinB ) { pfitB.pop_back(); fitter->fitVertex( pfitB, vfitB ); }
    } else {
      if( ! putinA ) { pfitA.pop_back(); fitter->fitVertex( pfitA, vfitA ); }
    }
    debug()<<"xxx3 sizeA="<<pfitA.size()<<" sizeB="<<pfitB.size()<<endreq;

    //debug
    debug() << "particle to test was:" <<(*jpp)->particleID().pid()
            << "  pt=" << (*kp)->pt()/GeV <<endreq;
    debug() << " zA=" <<zA<< " chiA="<< chiA << " put?"<<putinA<<endreq;
    debug() << " zB=" <<zB<< " chiB="<< chiB << " put?"<<putinB<<endreq;
    debug() << "vtxA: " << pfitA.size()<<endreq;
    for(ParticleVector::iterator kp=pfitA.begin(); kp!=pfitA.end(); ++kp){
      debug()<< "   " << (*kp)->particleID().pid()
             << " pt="<< (*kp)->pt()/GeV <<endreq;
    }
    debug() << "vtxB: " << pfitB.size()<<endreq;
    for(ParticleVector::iterator kp=pfitB.begin(); kp!=pfitB.end(); ++kp){
      debug()<< "   " << (*kp)->particleID().pid()
             << " pt="<< (*kp)->pt()/GeV <<endreq;
    }

    //if the part was not added skip the rest
    if( ! putinA ) if( ! putinB ) continue;
    if( m_SkipTrackPurge ) continue;

    //otherwise look what is the part which behaves worse in A
    if(putinA) if(pfitA.size() > 3 ) {
      int ikpp = 0;
      double ipmax = -1.0;
      bool worse_exist = false;
      ParticleVector::iterator kpp, kpp_worse;
      for( kpp=pfitA.begin(); kpp!=pfitA.end(); kpp++, ikpp++ ){
        ParticleVector tmplist = pfitA;
        tmplist.erase( tmplist.begin() + ikpp );
        sc = fitter->fitVertex( tmplist, vtx ); 
        if( !sc ) continue;
        sc = geom->calcImpactPar(**kpp, vtx, ip, ipe, ipVec, errMatrix);
        if( !sc ) continue;
        if( ip/ipe > ipmax ) {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist ) {
        debug()<< "Worse=" << (*kpp_worse)->particleID().pid()
               << " P=" << (*kpp_worse)->p()/GeV
               << " ipmax=" << ipmax ;
        if ( ipmax > 3.0 ) {
          pfitA.erase( kpp_worse );
          debug() << " killed." << endreq;	
        } 
      }
      sc = fitter->fitVertex( pfitA, vfitA ); //RE-FIT//
      if( !sc ) pfitA.clear();
    }

    //same: look what is the part which behaves worse in B
    if(Bexist) if(putinB) if(pfitB.size() > 3 ) {
      int ikpp = 0;
      double ipmax = -1.0;
      bool worse_exist = false;
      ParticleVector::iterator kpp, kpp_worse;
      for( kpp=pfitB.begin(); kpp!=pfitB.end(); kpp++, ikpp++ ){
        ParticleVector tmplist = pfitB;
        tmplist.erase( tmplist.begin() + ikpp );
        sc = fitter->fitVertex( tmplist, vtx ); 
        if( !sc ) continue;
        sc = geom->calcImpactPar(**kpp, vtx, ip, ipe, ipVec, errMatrix);
        if( !sc ) continue;
        if( ip/ipe > ipmax ) {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist ) {
        debug()<< "Worse=" << (*kpp_worse)->particleID().pid()
               << " P=" << (*kpp_worse)->p()/GeV
               << " ipmax=" << ipmax ;
        if ( ipmax > 3.0 ) {
          pfitB.erase( kpp_worse );
          debug() << " killed." << endreq;	
        } 
      }
      sc = fitter->fitVertex( pfitB, vfitB ); //RE-FIT////////////////////
      if( !sc ) pfitB.clear();
    }
  }

  debug() << "=> Fit Results A: " << pfitA.size() <<endreq;
  if(Bexist) debug() << "=> Fit Results B: " << pfitB.size() <<endreq;
  vfitA.clearProducts();
  if(Bexist) vfitB.clearProducts();
  for( jp=pfitA.begin(); jp!=pfitA.end(); jp++ ) vfitA.addToProducts(*jp);
  if(Bexist) for(jp=pfitB.begin(); jp!=pfitB.end(); jp++) vfitB.addToProducts(*jp);
  if(pfitA.size()) vtxvect.push_back(vfitA);
  if(Bexist) if(pfitB.size()) vtxvect.push_back(vfitB);

  return vtxvect;
}
//=============================================================================
double SVertexABTool::ipprob(double x) {
  if( x > 40. ) return 0.6;
  double r = - 0.535 + 0.3351*x - 0.03102*pow(x,2) + 0.001316*pow(x,3)
    - 0.00002598*pow(x,4) + 0.0000001919*pow(x,5);
  if(r<0) r=0;
  return r;
}
double SVertexABTool::ptprob(double x) {
  if( x > 5.0 ) return 0.65;
  double r = 0.04332 + 0.9493*x - 0.5283*pow(x,2) + 0.1296*pow(x,3)
    - 0.01094*pow(x,4);
  if(r<0) r=0;
  return r;
}
double SVertexABTool::aprob(double x) {
  if( x < 0.02 ) return 0.32;
  return 0.4516 - 1.033*x;
}
//=============================================================================
ParticleVector SVertexABTool::toStdVector(SmartRefVector<Particle>& refvector){
  ParticleVector tvector(0);
  for( SmartRefVector<Particle>::iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
}
//=============================================================================
bool SVertexABTool::isinVtx( Vertex vtx, Particle* p) {  
  ParticleVector pvec = toStdVector(vtx.products());
  for( ParticleVector::iterator kp=pvec.begin(); kp!=pvec.end(); ++kp ){
    if( (*kp) == p ) return true;
  }
  return false;
}

//=============================================================================



