// $Id: PythiaProduction.h,v 1.3 2005-11-08 00:08:42 robbep Exp $
#ifndef LBPYTHIA_PYTHIAPRODUCTION_H 
#define LBPYTHIA_PYTHIAPRODUCTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"

class IBeamTool ;

/** @class PythiaProduction PythiaProduction.h 
 *  
 *  Interface tool to produce events with Pythia
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-16
 */
class PythiaProduction : public GaudiTool, virtual public IProductionTool {
public:
  typedef std::vector< std::string > CommandVector ;

  /// Standard constructor
  PythiaProduction( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~PythiaProduction( ); ///< Destructor

  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode finalize( ) ;   ///< Finalize method

  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    HardInfo * theInfo ) ;

  virtual void setStable( const ParticleProperty * thePP ) ;

  virtual void updateParticleProperties( const ParticleProperty * thePP ) ;

  virtual void turnOnFragmentation( ) ;
  
  virtual void turnOffFragmentation( ) ;

  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                HardInfo * theInfo ) ;
  
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) ;

  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) ;

  virtual void printRunningConditions( ) ;

  virtual bool isSpecialParticle( const ParticleProperty * thePP ) ;

protected:
  /// Parse Pythia commands from a string vector
  StatusCode parsePythiaCommands( const CommandVector & theVector ) ;

  /// Print Pythia parameters
  void printPythiaParameter( ) ;
  
  /// Retrieve hard process information
  void hardProcessInfo(HardInfo* hardInfo) ;

private:
  std::string m_frame   ;  ///< FRAME string
  std::string m_beam    ;  ///< BEAM string
  std::string m_target  ;  ///< TARGET string
  double m_win          ;  ///< WIN

	CommandVector m_userPythiaCommandVector ; ///< User Commands to setup pythia
  CommandVector m_basePythiaCommandVector ; ///< Basic commands to setup Pythia

  int m_eventListingLevel ;
  int m_initializationListingLevel ;
  int m_finalizationListingLevel ;

  std::string m_beamToolName ;

  std::string m_pythiaListingFileName ;
  int m_pythiaListingUnit ;

  bool m_variableEnergy ;

  int m_nEvents ; ///< Internal event counter

  IBeamTool * m_beamTool ;
};
#endif // LBPYTHIA_PYTHIAPRODUCTION_H
