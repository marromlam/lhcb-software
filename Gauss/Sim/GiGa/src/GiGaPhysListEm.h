#ifndef    GIGA_GiGaPhysListEM_H
#define    GIGA_GiGaPhysListEM_H 1 


#include "GiGa/GiGaPhysList.h" 


template <class PL> class GiGaPhysListFactory;

/** @class  GiGaPhysListEm        GiGaPhysListEm.h  GiGaPhysListEm.h
    
    "Real" implementation of "Physics List"                                            
    it correspond("cut&paste") to novice example N02.
    
    In this example, all the so called 'electromagnetic processes' are
    introduced for gamma, charged leptons, and charged hadrons (see the
    method PhysicsList::ConstructEM()). 
    
    @author Vanya Belyaev
*/


class GiGaPhysListEm : public GiGaPhysList
{
  ///
  friend class GiGaPhysListFactory<GiGaPhysListEm>;
  ///
 protected:
  ///
  GiGaPhysListEm( const std::string& nick , ISvcLocator* loc );
  virtual ~GiGaPhysListEm();
  ///
  
  /// from G4VUserPhysicsList 
  void ConstructParticle ();
  void ConstructProcess  ();
  void SetCuts           ();
  ///
  
 private:
  
  // these methods Construct particles 
  void ConstructBosons   ();
  void ConstructLeptons  ();
  void ConstructMesons   ();
  void ConstructBaryons  ();
  
  // these methods Construct physics processes and register them
  void ConstructGeneral  ();
  void ConstructEM       ();
  

 private:
  ///
  GiGaPhysListEm();
  GiGaPhysListEm           ( const GiGaPhysListEm& );
  GiGaPhysListEm& operator=( const GiGaPhysListEm& );
  ///
};


#endif     GIGA_GiGaPhysListEM_H
