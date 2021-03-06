// $Id: DecayTreeTupleBase.h,v 1.10 2009-03-27 17:51:36 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLEBASE_H
#define JBOREL_DECAYTREETUPLEBASE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"
#include "DecayTreeTupleBase/OnePart.h"
#include "DecayTreeTupleBase/ITupleToolDecay.h"
#include "Kernel/IEventTupleTool.h"
#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IMCParticleTupleTool.h"
#include "boost/lexical_cast.hpp"
#include "Kernel/Escape.h"

#include "Event/MCParticle.h"
#include "LoKi/IMCDecay.h"
#include "LoKi/IDecay.h"

/** @class DecayTreeTupleBase DecayTreeTupleBase.h jborel/DecayTreeTupleBase.h
 *
 * Base class for algorithms providing a DecayTreeTuple
 *
 * \sa DecayTreeTuple, MCDecayTreeTuple
 *
 * \author Patrick Koppenburg based on Jérémie Borels DecayTreeTuple
 * \date 2009-01-20
 */
class DecayTreeTupleBase : public DaVinciTupleAlgorithm
{

public:

  /// Standard constructor
  DecayTreeTupleBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecayTreeTupleBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Access the head decay name
  const std::string& headDecay() const { return m_headDecay; }

  /// Access the tuple name
  const std::string& tupleName() const { return m_tupleName; }

  /// Access the decay finder tool
  IMCDecayFinder* mcdkFinder() const { return m_mcdkFinder; }

  /// Access the MC decay finder tool
  IDecayFinder*   dkFinder()   const { return m_dkFinder; }

  bool useLoKiDecayFinders() const { return m_useLoKiDecayFinders; }
  const Decays::IMCDecay::iTree& mcDecayTree() const { return m_mcdecayTree; }
  const Decays::IDecay::iTree& decayTree() const { return m_decayTree; }

  /// Initialize the main decay
  bool initializeDecays(const bool isMC);

  //! Retrieve from the local storage all the top level particles that match
  //! the decay descriptor
  bool getDecayMatches( const LHCb::Particle::ConstVector& src, LHCb::Particle::ConstVector& target );

  //! Retrieve from the local storage all the top level particles that match
  //! the decay descriptor
  bool getDecayMatches( const LHCb::MCParticle::ConstVector& src, LHCb::MCParticle::ConstVector& target );

private:

  //! Call the fill methode which does not take a particle as argument
  StatusCode fillEventRelatedVariables( Tuples::Tuple& );

  //! Check if ready to fill or trigger the initialization
  //! this is where all the difference between MC particle and Particle occurs
  void matchSubDecays( const LHCb::Particle::ConstVector& ,
                       LHCb::Particle::ConstVector& ,
                       const ITupleToolDecay* );

  //! Check if ready to fill or trigger the initialization
  //! this is where all the difference between MC particle and Particle occurs
  void matchSubDecays( const LHCb::MCParticle::ConstVector& ,
                       LHCb::MCParticle::ConstVector& ,
                       const ITupleToolDecay*  );

  /// Call successively all OnePart's fill methods
  bool fillOnePart( Decays::OnePart*, 
                    Tuples::Tuple&, 
                    const LHCb::Particle* mother, 
                    const LHCb::Particle* );

  /// Call successively all OnePart's fill methods
  bool fillOnePart( Decays::OnePart*, 
                    Tuples::Tuple&, 
                    const LHCb::MCParticle* mother,
                    const LHCb::MCParticle* );

  /// check for unicity of names
  bool checkUnicity() const ;

  /// print infos
  void printInfos() const ;

  /// get daughters for Particles trivially
  LHCb::Particle::ConstVector daughtersVector(const LHCb::Particle* d) const
  {
    if (!d) Exception("NULL Particle");
    return d->daughtersVector() ;
  }

  /// get daughters for MCParticles, not so trivially
  LHCb::MCParticle::ConstVector daughtersVector(const LHCb::MCParticle* d) const ;

  /// Switch for initializeStufferTools
  void switchStufferTools(const LHCb::MCParticle*)
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Initialize MCParticle tools" << endmsg ;
    initializeStufferTools(m_mcTools);
  }

  /// Switch for initializeStufferTools
  void switchStufferTools(const LHCb::Particle*)
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Initialize Particle tools" << endmsg ;
    initializeStufferTools(m_pTools);
  }

  /// has oscillated (needed to tell if one needs reverting)
  bool hasOscillated(const LHCb::Particle*) const { return false; }

  /// has oscillated (needed to tell if one needs reverting)
  bool hasOscillated(const LHCb::MCParticle* P) const
  {
    return ( P ? P->hasOscillated() : false );
  }

  /// Get a list of the event tools
  std::vector<std::string> getEventTools() const;

  /// Get branch name for given particle
  std::string getBranchName( const std::string& realname ) const;

protected:

  std::vector<std::string> m_toolList;

private:

  IMCDecayFinder* m_mcdkFinder; ///< MC truth decay finder

  IDecayFinder* m_dkFinder;     ///< Decay finder

  bool m_useLoKiDecayFinders;
  Decays::IMCDecay::Tree m_mcdecayTree; ///< MC truth decay tree
  Decays::IDecay::Tree   m_decayTree;   ///< decay tree

  std::string m_headDecay;

  std::string m_tupleName;

  std::map< std::string, std::string > m_decayMap;
  std::vector<ITupleToolDecay*> m_decays;

  bool m_useLabName; ///< use labX_ as particle name
  bool m_tupleNameAsToolName;

  /// force the mother to have a positive ID and revert all other particles.
  /// This is a neat alternative to m_useLabName
  bool m_revertToPositiveID ;

  std::vector<Decays::OnePart*> m_parts;

  std::vector< IMCParticleTupleTool* > m_mcTools;
  std::vector< IEventTupleTool* > m_eTools;
  std::vector< IParticleTupleTool* > m_pTools;

  //=============================================================================
  // Templated methods accessed from other classes need to be in the header
  // to make sure the linker builds them.
  //=============================================================================
protected:
#include "DecayTreeTupleBaseTemplates.icpp"

};

#endif // JBOREL_DECAYTREETUPLEBASE_H
