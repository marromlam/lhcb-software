#ifndef __VTKDECAYCHAIN_HH__
#define __VTKDECAYCHAIN_HH__

#include <map>
#include "ParticleBase.h"
#include "MergedConstraint.h"

class LHCb::Particle ;

namespace vtxtreefit {

  class FitParams ;
  class ParticleBase ;

  class DecayChain
  {
  public:
    DecayChain() : m_mother(0) {}

    DecayChain(const LHCb::Particle& bc, bool forceFitAll=false)  ;
    ~DecayChain() ;
    
    int dim() const { return m_dim ; }

    void initConstraintList() ;
    ErrCode init(FitParams& par) ;
    ErrCode filter(FitParams& par, bool firstpass=true) ;
    double chiSquare(const FitParams* par) const ;

    ParticleBase* mother() { return m_mother ; }
    const ParticleBase* cand() { return m_cand ; }
    const ParticleBase* mother() const { return m_mother ; }
    const ParticleBase* locate(const LHCb::Particle& bc) const ;

    int index(const LHCb::Particle& bc) const ;
    int posIndex(const LHCb::Particle& bc) const ;
    int momIndex(const LHCb::Particle& bc) const ;
    int tauIndex(const LHCb::Particle& bc) const ;
    void setOwner(bool b) { m_isOwner=b ;}
    int momIndex() const ;

    void printConstraints(std::ostream& os=std::cout) const ;
    void setMassConstraint( const LHCb::Particle& bc, bool add=true) ;

  private:
    int m_dim ;
    ParticleBase* m_mother ;     // head of decay tree
    const ParticleBase* m_cand ; // fit candidate (not same to mother in case of bs/be constraint)
    ParticleBase::constraintlist m_constraintlist ;
    std::vector<Constraint*> m_mergedconstraintlist ;
    MergedConstraint mergedconstraint ;
    typedef std::map<const LHCb::Particle*,const ParticleBase*> ParticleMap ;
    mutable ParticleMap m_particleMap ;
    bool m_isOwner ;
  } ;

}



#endif
