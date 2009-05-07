#include "FitParams.h"
#include "InteractionPoint.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

namespace vtxtreefit
{
  extern int vtxverbose ;

  InteractionPoint::InteractionPoint(const LHCb::Vertex& ipvertex,
				     const LHCb::Particle& daughter, 
				     bool forceFitAll)
    : ParticleBase("IP"),
      m_ipPos(3,0), m_ipCov(3,1), m_ipCovInv(3,1) 
  {
    m_daughter = ParticleBase::createParticle(daughter,this,forceFitAll) ;

    std::cout << "VtkInteractionPoint: still using dummy beamspot"<< std::endl ;
    m_ipCov(1,1) = 0.1 ;
    m_ipCov(2,2) = 0.1 ;
    m_ipCov(3,3) = 50 ;
    int ierr ;
    m_ipCovInv = m_ipCov.inverse(ierr) ;
    
    if(vtxverbose>=2)
      std::cout << "VtkInteractionPoint: initial beam spot = (" 
		<<m_ipPos(1) << "," << m_ipPos(2) << "," << m_ipPos(3) << ")" << std::endl ;
  }

  InteractionPoint::~InteractionPoint() {}

  ErrCode 
  InteractionPoint::initPar1(FitParams* fitparams)
  {
    ErrCode status ;
    int posindex = posIndex() ;
    for(int row=1; row<=3; ++row)
      fitparams->par()(posindex+row) = m_ipPos(row) ;
    
    status |= m_daughter->initPar1(fitparams) ;
    status |= m_daughter->initPar2(fitparams) ;
    
    return status ;
  }

  ErrCode 
  InteractionPoint::initPar2(FitParams* /*fitparams*/)
  {
    // nothing left to do: actually, should never be called
    assert(0) ;
    return ErrCode::success ;
  }

  ErrCode
  InteractionPoint::initCov(FitParams* fitpar) const 
  {
    ErrCode status ;
    int posindex = posIndex() ;
    for(int row=1; row<=3; ++row)
      fitpar->cov().fast(posindex+row,posindex+row) 
	= 1000*m_ipCov.fast(row,row) ;
    status |= m_daughter->initCov(fitpar) ;

    return status ;
  }

  ErrCode
  InteractionPoint::projectIPConstraint(const FitParams& fitparams, 
					Projection& p) const
  { 
    int posindex = posIndex() ;
    int maxrow = 3 ;
    for(int row=1; row<=maxrow; ++row) {
      p.r(row) = fitparams.par()(posindex+row) - m_ipPos(row) ;
      p.H(row,posindex+row) = 1 ;
      for(int col=1; col<=row; ++col)
	p.Vfast(row,col) = m_ipCov.fast(row,col) ;
    }
    return ErrCode::success ;
  }

  ErrCode 
  InteractionPoint::projectConstraint(Constraint::Type type, 
				      const FitParams& fitparams, 
				      Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::beamspot:
      status |= projectIPConstraint(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  double 
  InteractionPoint::chiSquare(const FitParams* fitparams) const
  {
    // calculate the chi2
    int posindex = posIndex() ;
    HepVector residual = m_ipPos - fitparams->par().sub(posindex+1,posindex+3) ;
    double chisq = m_ipCovInv.similarity(residual) ;

    // add the daughters
    chisq += ParticleBase::chiSquare(fitparams) ;

    return chisq ;
  }

  void InteractionPoint::addToConstraintList(constraintlist& alist, int depth) const
  {
    // first the beamspot
    int dim = 3 ;
    alist.push_back(Constraint(this,Constraint::beamspot,depth,dim)) ;
    
    // then the daughter
    m_daughter->addToConstraintList(alist,depth+1) ;
  }

}


