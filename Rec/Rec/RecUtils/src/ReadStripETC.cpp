// $Id: ReadStripETC.cpp,v 1.1.1.1 2006-07-04 15:59:34 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// event model
#include "Event/SelResult.h"

// local
#include "ReadStripETC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReadStripETC
//
// 2006-06-26 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ReadStripETC );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReadStripETC::ReadStripETC( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_usedSelectionsFilled(false)
  , m_longType(8)
{
  m_vetoedFields.push_back("entry");
  m_vetoedFields.push_back("nChargedProto");
  m_vetoedFields.push_back("nCharged");
  m_vetoedFields.push_back("nPrim");
  
  declareProperty( "CollectionName", m_collectionName = "<not set>");
  declareProperty( "VetoedFields", m_vetoedFields);
  declareProperty( "Selections", m_selections); /// empty means all

}
//=============================================================================
// Destructor
//=============================================================================
ReadStripETC::~ReadStripETC() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ReadStripETC::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

  debug() << "==> Initialize" << endmsg;
  if ( m_selections.empty() ) {
    info() << "Will be writing out SelResults for all selections except " ;
    for ( std::vector<std::string>::const_iterator i = m_vetoedFields.begin() ;
          i != m_vetoedFields.end(); ++i ) info() << *i << " " ;
    info() << endmsg ;
  } else {
    info() << "Will be writing out SelResults for  " ;
    for ( std::vector<std::string>::const_iterator i = m_selections.begin() ;
          i != m_selections.end(); ++i ) info() << *i << " " ;
    info() << endmsg ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ReadStripETC::execute() {

  debug() << "==> Execute" << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;

  // it's a typedef SmartDataPtr<NTuple::Tuple> NTuplePtr
  NTuplePtr nt(evtColSvc(), "EventSelector_1/TagCreator/1" );

  if ( !nt ) {
    err() << "Nothing at " << "EventSelector_1/"+m_collectionName+"/1" << endmsg ;
    return StatusCode::FAILURE ;
  } else {
    if ( !m_usedSelectionsFilled ) sc = fillUsedSelections(nt);
    if (sc) sc = fillSelResults(nt); 
  }
  return sc ;
}

//=============================================================================
// First filling of to be used fields
//=============================================================================
StatusCode ReadStripETC::fillUsedSelections(NTuplePtr& nt){
  m_usedSelectionsFilled = true ;
  const INTuple::ItemContainer ic = nt->items() ;
  debug() << "There are " << ic.size() << " items in nTuple" << endmsg ;

  bool taken = false ;
  for ( INTuple::ItemContainer::const_iterator i=ic.begin() ; i!=ic.end() ; ++i){
    verbose() << "There is item " << (*i)->name() << " of type " << (*i)->type() << endmsg ;
    if ( (*i)->type()==m_longType ){ // long
      if ( m_selections.empty() ){
        taken = true ;
        for ( std::vector<std::string>::const_iterator iv = m_vetoedFields.begin() ;
              iv != m_vetoedFields.end(); ++iv ) {
          if ( (*i)->name() == *iv ) taken = false ;
        }
      } else {
        for ( std::vector<std::string>::const_iterator is = m_selections.begin() ;
              is != m_selections.end(); ++is ) {
          if ( (*i)->name() == *is ) taken = true ;
        }
      }
      if (taken) {    
        debug() << "  Taking " << (*i)->name() << endmsg ;
        m_usedSelections.push_back((*i)->name());
      } else {
        debug() << "Skipping " << (*i)->name() << endmsg ;
      }
    }
  }
  info() << "Will be writing out SelResults for  " ;
  for ( std::vector<std::string>::const_iterator i = m_usedSelections.begin() ;
        i != m_usedSelections.end(); ++i ) info() << *i << " " ;
  info() << endmsg ;
  
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Filling of SelResults
//=============================================================================
StatusCode ReadStripETC::fillSelResults(NTuplePtr& nt)const{
  
  LHCb::SelResults* resultsToSave ;
  std::string location = LHCb::SelResultLocation::Default;
  // Check if SelResult contained has been registered by another algorithm
  if ( exist<LHCb::SelResults>(location) ){
    verbose() << "SelResult exists already " << endmsg ;
    resultsToSave = get<LHCb::SelResults>(location);
  } else {
    verbose() << "Putting new SelResult container " << endmsg ;
    resultsToSave = new LHCb::SelResults();
    put(resultsToSave,location);
  }

  for ( std::vector<std::string>::const_iterator i = m_usedSelections.begin() ;
        i != m_usedSelections.end(); ++i ) {
    NTuple::Item<long> val;
    debug() << "There is item " << *i << endmsg ;
    StatusCode status = nt->item(*i, val);
    if ( status ) debug() << "Value of " << *i << " is " << val << endmsg ;
    else {
      err() << "Could not read value of " << *i << endmsg ;
      return StatusCode::FAILURE;
    }
    LHCb::SelResult* myResult = new LHCb::SelResult();
    myResult->setFound((val!=0));
    myResult->setLocation( ("/Event/Phys/"+*i));
    verbose() << "SelResult location set to " << "/Event/Phys/"+*i << endmsg;
    resultsToSave->insert(myResult);
  }
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode ReadStripETC::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
