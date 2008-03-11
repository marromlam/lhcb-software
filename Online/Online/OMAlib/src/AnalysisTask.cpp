// $Id: AnalysisTask.cpp,v 1.2 2008-03-11 18:23:26 ggiacomo Exp $


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// HistDB
#include "OnlineHistDB/OnlineHistDB.h"
// local
#include "OMAlib/OMAlib.h"
#include "OMAlib/AnalysisTask.h"
#include "OMAlib/SavesetFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AnalysisTask
//
// 2008-02-18 : Giacomo Graziani
//-----------------------------------------------------------------------------



AnalysisTask::AnalysisTask( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : OMAMsgInterface(),
    GaudiAlgorithm ( name , pSvcLocator ),
    m_histDB (NULL),
    m_inputFiles(0),
    m_inputTasks(0)
{
  declareProperty ( "useHistDB"    , m_useDB = false);
  declareProperty ( "HistDB"       , m_DB = OnlineHistDBEnv_constants::DB );
  declareProperty ( "HistDBuser"   , m_DBuser = OnlineHistDBEnv_constants::ACCOUNT );
  declareProperty ( "HistDBpw"     , m_DBpw = OnlineHistDBEnv_constants::PASSWORD );
  declareProperty ( "InputFiles"   , m_inputFiles);
  declareProperty ( "InputTasks"   , m_inputTasks);
  declareProperty ( "MessageMode"  , m_MessageMode = 0 );
}

AnalysisTask::~AnalysisTask() {
  delete m_omalib;
  std::vector<SavesetFinder*>::iterator iSS;
  for(iSS = m_saveset.begin() ; iSS != m_saveset.end() ; iSS++) {
    delete (*iSS);
  }
} 


StatusCode AnalysisTask::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;  

  // where to send messages 
  msgMode = (OMAMsgMode) m_MessageMode;

  // instantiate OMAlib with or without HistDB
  if(m_useDB)
    m_omalib = new OMAlib( m_DBpw, m_DBuser, m_DB );
  else
    m_omalib = new OMAlib( (OnlineHistDB*) NULL );

  m_histDB = m_omalib->dbSession();
  m_omalib->setMessageMode(msgMode);


  if ( ! m_inputFiles.empty() ) {
    // single shot: analyze these files now 
    std::vector<std::string>::iterator iF;
    for(iF =  m_inputFiles.begin() ; iF != m_inputFiles.end() ; iF++) {
      info() <<"Analyzing histograms on file "<<*iF<<endmsg;
      analyze( *iF , "any" );
    }
  }
  else {
    std::vector<std::string>::iterator iF;
    for(iF =  m_inputTasks.begin() ; iF != m_inputTasks.end() ; iF++) {
      m_saveset.push_back( new SavesetFinder(this, *iF) );
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode AnalysisTask::execute() {
  return StatusCode::SUCCESS;
}


StatusCode AnalysisTask::finalize() {
  return GaudiAlgorithm::finalize();  
}


