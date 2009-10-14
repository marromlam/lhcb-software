// $Id: TupleToolMCTruth.cpp,v 1.15 2009-10-14 14:01:28 jpalac Exp $
// Include files
#include "gsl/gsl_sys.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
// local
#include "TupleToolMCTruth.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/MCParticle.h"

// kernel
#include "Kernel/IParticle2MCAssociator.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolMCTruth
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMCTruth );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCTruth::TupleToolMCTruth( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_p2mcAssoc(0)
    , m_p2mcAssocType("DaVinciSmartAssociator")
    , m_toolList(0)
    , m_mcTools(0)

{
  declareInterface<IParticleTupleTool>(this);

  // The names of MCTupleTools to use on the associated mcp
  declareProperty( "ToolList", 
                   m_toolList = std::vector<std::string>(1,
                                                         "MCTupleToolKinematic") );
  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType);
  
}

//=============================================================================

StatusCode TupleToolMCTruth::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, 
                                             this);

   //initialise the tools
  std::sort( m_toolList.begin(), m_toolList.end() );
  std::unique( m_toolList.begin(), m_toolList.end() );
  for( std::vector<std::string>::const_iterator it = m_toolList.begin(); m_toolList.end()!=it ; ++it )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Adding the tool " << *it << endmsg ;
    IMCParticleTupleTool* aTool=tool<IMCParticleTupleTool>(*it,this);
    if(aTool) m_mcTools.push_back(aTool);
    else Warning("There was a problem retrieving " + *it +" , this tool will be ignored");
  }
  
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Completed TupleTool intialisation, " 
              << m_mcTools.size() 
              << " tools added " << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TupleToolMCTruth::fill( const LHCb::Particle* 
				 , const LHCb::Particle* P
				 , const std::string& head
				 , Tuples::Tuple& tuple ){

  Assert( m_p2mcAssoc 
          , "The DaVinci smart associator hasn't been initialized!");
  
  
  int mcPid=0;
  bool test = true;
  const LHCb::MCParticle* mcp(0);
  
  if( P ){
    //assignedPid = P->particleID().pid();
    if (msgLevel(MSG::VERBOSE)) verbose() << "MCTupleToolKinematic::getting related MCP to " << P << endmsg ;
    mcp = m_p2mcAssoc->relatedMCP(P);
    if (msgLevel(MSG::VERBOSE)) verbose() << "MCTupleToolKinematic::got mcp " << mcp << endmsg ;
  }
 
  // pointer is ready, prepare the values:
  if( mcp )
  {
    mcPid=mcp->particleID().pid();
  }
  
  // fill the tuple:
  test &= tuple->column( head+"_TRUEID", mcPid );  

  //fill all requested MCTools
  for(std::vector< IMCParticleTupleTool* >::const_iterator it=m_mcTools.begin(); it!=m_mcTools.end(); it++)
    test &=(*it)->fill(NULL,mcp,head,tuple);
  
  return StatusCode(test);
}

