// $Id: TupleToolMCTruth.h,v 1.8 2009-10-14 14:01:28 jpalac Exp $
#ifndef TUPLETOOLMCTRUTH_H
#define TUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Kernel/IParticleTupleTool.h"            // Interface

//struct Particle2MCLinker;
#include <vector>

class IParticle2MCAssociator;

/** @class TupleToolMCTruth TupleToolMCTruth.h
 *
 * \brief Fill MC truth info if a link is present
 *
 * Uses an IParticle2MCAssociator to perform the association.
 * <b> Properties: </b>
 *
 * IP2MCPAssociatorType: Implementation of IP2MCAssociator to be used. Default: DaVinciSmartAssociator.
 * 
 * ToolList: List of MCTupleTools to run. Default: [MCTupleToolKinematic] 
 *
 * - head_TRUEID : true pid
 *
 
 * To add more entries, add the appropriate MCTupleTool

 * Configure the option ToolList to add MCTupleTools

 * The MCAssociation is run only once, then these tuple tools are called


 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 *  2008-09-23 Adlene Hicheur - Added true angles information for P2VV
 *  2009-06-03 Rob Lambert - Major Changes
 */

class TupleToolMCTruth : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolMCTruth( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolMCTruth(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

  virtual StatusCode initialize();
  //  virtual StatusCode finalize();

private:

  IParticle2MCAssociator* m_p2mcAssoc;
  std::string m_p2mcAssocType;
  std::vector< std::string > m_toolList;///<names of all MCTupleTools, set by the option ToolList
  
  std::vector< IMCParticleTupleTool* > m_mcTools;///<vector of MCTools to fill
  
};

#endif // TUPLETOOLMCTRUTH_H
