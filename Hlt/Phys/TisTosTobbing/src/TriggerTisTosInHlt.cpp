// $Id: TriggerTisTosInHlt.cpp,v 1.2 2009-12-23 17:59:51 graven Exp $
// Include files 
#include <algorithm>
#include <vector>

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TriggerTisTosInHlt.h"

#include "boost/regex.hpp"

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

#include "HltBase/IHltDataSvc.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTisTosInHlt
//
// 2007-08-20 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerTisTosInHlt );

const std::vector< std::string > TriggerTisTosInHlt::m_empty_selections = std::vector< std::string >();

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerTisTosInHlt::TriggerTisTosInHlt( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TriggerSelectionTisTosInHlt ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);

  declareProperty("TriggerInputWarnings", m_trigInputWarn = false );
  declareProperty("AllowIntermediateSelections", m_allowIntermediateSelections = false );

}
//=============================================================================
// Destructor
//=============================================================================
TriggerTisTosInHlt::~TriggerTisTosInHlt() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerTisTosInHlt::initialize() {
  StatusCode sc = TriggerSelectionTisTosInHlt::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  setOfflineInput();
  setTriggerInput();
  
  return StatusCode::SUCCESS;
}

//=============================================================================


void TriggerTisTosInHlt::getTriggerNames()
{
  if( m_newEvent ){
    m_triggerNames.clear(); 
    m_newEvent = false;
  }

  // done before ?
  if( !(m_triggerNames.empty()) ){ return; }


  // get trigger names from HltDecReports and HltSelReports
  if( !m_hltDecReports )getHltSummary();
  //    for the same TCK this should be fixed list for events which passed Hlt
  if( m_hltDecReports ){
    m_triggerNames = m_hltDecReports->decisionNames();
  }

  if( m_allowIntermediateSelections ){    
    std::vector<Gaudi::StringKey> selIDs = m_hltDataSvc->selectionKeys();
    for( std::vector<Gaudi::StringKey>::const_iterator i =
          selIDs.begin(); i!=selIDs.end(); ++i) {
      std::string name=i->str();      
      if( find( m_triggerNames.begin(), m_triggerNames.end(), name )
          == m_triggerNames.end() ){
        m_triggerNames.push_back(name);
      }
    }
  }
  
  if( m_triggerNames.empty() ){
    Warning( "No known trigger names found" , StatusCode::FAILURE, 1 ).setChecked();
  }
  
}

void TriggerTisTosInHlt::setTriggerInput()
{
  m_triggerInput_Selections.clear();
}

void TriggerTisTosInHlt::addToTriggerInput( const std::string & selectionNameWithWildChar)
{
  // if selectionNameWithWildChar contains a * without a . in front of it: print warning...
  static boost::regex warn("[^\\.]\\*");
  if ( boost::regex_search( selectionNameWithWildChar, warn )  ) {
        Warning( " addToTriggerInput now does Posix (Perl) regular expression matching instead of globbing;" 
                 " this implies that eg. a '*' should be replaced by '.*'. You've specified "
                 " a selectionName using a '*', without leading '.': '" + selectionNameWithWildChar 
               + "'. Please verify whether this is what you still want\n "
               " For more information on the supported syntax, please check "
               " http://www.boost.org/doc/libs/1_39_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html"
                 ,StatusCode::SUCCESS 
                 ).ignore();
  }
  unsigned int sizeAtEntrance( m_triggerInput_Selections.size() );
  getTriggerNames();
  boost::regex expr(selectionNameWithWildChar);
  for( std::vector< std::string >::const_iterator inpt=m_triggerNames.begin();inpt!=m_triggerNames.end();++inpt){
    if( boost::regex_match( *inpt, expr ) ) {
      if( find( m_triggerInput_Selections.begin(), m_triggerInput_Selections.end(), *inpt ) 
          == m_triggerInput_Selections.end() ){
        m_triggerInput_Selections.push_back(*inpt);
      }
    }
  }
  if( m_trigInputWarn && (m_triggerInput_Selections.size()==sizeAtEntrance) ){
    std::ostringstream mess;
    mess << " addToTriggerInput called with selectionNameWithWildChar=" << selectionNameWithWildChar
         << " added no selection to the Trigger Input, which has size=" << m_triggerInput_Selections.size();
    Warning( mess.str(),StatusCode::SUCCESS, 50 ).setChecked();
  }
}
 
std::vector< std::string > TriggerTisTosInHlt::triggerSelectionNames(unsigned int decisionRequirement,
                                                                  unsigned int tisRequirement,
                                                                  unsigned int tosRequirement )
{ 
  if( (decisionRequirement>=kAnything) && ( tisRequirement>=kAnything) && ( tosRequirement>=kAnything) )
  {
    return m_triggerInput_Selections;
  }
  std::vector< std::string > selections;  
  for( std::vector<std::string>::const_iterator iSel = m_triggerInput_Selections.begin();
       iSel != m_triggerInput_Selections.end(); ++iSel ){
    bool decision,tis,tos;
    selectionTisTos( *iSel,decision,tis,tos);
    if( ((decisionRequirement>=kAnything)||(decision==decisionRequirement))
        && ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
        && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
      selections.push_back( *iSel );
    }
  }
  return selections;  
}


void TriggerTisTosInHlt::triggerTisTos( bool & decision, bool & tis, bool & tos)
{
  decision = false; tis=false; tos=false;
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" triggerTisTos called with empty Trigger Input").setChecked();
    return;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    bool decisionThis, tisThis, tosThis;
    selectionTisTos( *iTriggerSelection, decisionThis, tisThis, tosThis );
    if( decisionThis ){      
      //      decision = decision || decisionThis;
      decision = true;      
      tis = tis || tisThis;
      tos = tos || tosThis;
    }    
    if( tis && tos ){ break; }
  }
}

std::vector<const LHCb::HltObjectSummary*> TriggerTisTosInHlt::hltObjectSummaries( unsigned int tisRequirement,
                                                                              unsigned int tosRequirement)
{
  std::vector<const LHCb::HltObjectSummary*> hosVec;  
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" hltObjectSummaries called with empty Trigger Input").setChecked();
    return hosVec;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    std::vector<const LHCb::HltObjectSummary*> selHosVec = 
      hltSelectionObjectSummaries(*iTriggerSelection,tisRequirement,tosRequirement);
    hosVec.insert(hosVec.end(),selHosVec.begin(),selHosVec.end());
  }
  return hosVec;
}

  
