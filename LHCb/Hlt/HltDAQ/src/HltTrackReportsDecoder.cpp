// Include files 
#include "boost/format.hpp"
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"

// local
#include "HltTrackReportsDecoder.h"
#include "HltTrackReportsWriter.h"
#include "HltTrackingCoder.h"

#include "Event/Track.h"


using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackReportsDecoder
//
// The actual decoding of tracks is delegated 
// to the functions in HltTrackingCoder.
//
// 2014-01-16 : Sebastian Neubert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackReportsDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackReportsDecoder::HltTrackReportsDecoder( const std::string& name,
                                          ISvcLocator* pSvcLocator)
: Decoder::AlgBase ( name , pSvcLocator )
{
   //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();

  declareProperty("OutputLocation",
    m_outputLocation= "/Hlt/Track/Velo" );  


  declareProperty("SourceID",
		  m_sourceID= HltTrackReportsWriter::kSourceID_Dummy );  

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackReportsDecoder::~HltTrackReportsDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackReportsDecoder::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  //m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");

  if( m_sourceID > HltTrackReportsWriter::kSourceID_Max ){
    m_sourceID = m_sourceID & HltTrackReportsWriter::kSourceID_Max;
    return Error("Illegal SourceID specified (too large);" , StatusCode::FAILURE, 50 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get inputs
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  if( ! rawEvent ){
    return Error(" No RawEvent found at any location. No HltTracks created.");
  }  

  // check if the container is already present
  
 
  // create output container and put it on TES
  LHCb::Tracks* outputTracks = NULL;
  try {
    outputTracks=getOrCreate<LHCb::Tracks,LHCb::Tracks>(m_outputLocation);
    //put( outputTracks, m_outputLocation );
  }
  catch(GaudiException ex) {
    warning() << "Failed to create output location " <<  m_outputLocation << endmsg;
    warning() << ex.message() << endmsg;
    return StatusCode::FAILURE;
  }

  if(outputTracks->size()>0) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "outputLocation already contains tracks. Skipping decoding." << endmsg;
    return StatusCode::SUCCESS;
  } 


  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltTrackReportsRawBanks = rawEvent->banks( RawBank::HltTrackReports);
  if( !hltTrackReportsRawBanks.size() ){
    return Warning( " No HltTrackReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }

  // Check version number to make sure we are on the same page here (this should go to the decoder function?)
   const RawBank* hltTrackReportsRawBank0 = *(hltTrackReportsRawBanks.begin());
   if( hltTrackReportsRawBank0->version() > kVersionNumber ){
     Warning( " HltTrackReportsReports RawBank version is higher than expected. Will try to decode it anyway." ,StatusCode::SUCCESS, 20 );
   }

   // -------------------------------------------------------
   // do the actual decoding: see HltTrackingCoder.cpp
   // -------------------------------------------------------

   for(RawBank* bank : hltTrackReportsRawBanks){
     if(bank->sourceID()==m_sourceID)decodeTracks(bank->data(),bank->size()/sizeof(unsigned int),outputTracks);
   }
  
   // for debug purposes print the contents of the outputLocation
   if( msgLevel(MSG::VERBOSE) )
     {
       verbose() << "----------------------------------------\n";
       verbose() << " Resurrected tracks: \n"; 
       LHCb::Tracks::const_iterator pItr;
       for(pItr = outputTracks->begin(); outputTracks->end() != pItr; ++pItr){
	 LHCb::Track* Tr = (*pItr);
	 verbose()  << *Tr << endmsg ;
	 // also dump IDs
	 verbose() << "LHCbIDs: [\n"; 
	 unsigned int nhits= Tr->nLHCbIDs();
	 for(unsigned int i=0;i<nhits;++i){
	   verbose() << Tr->lhcbIDs()[i] << ",\n";
	 }
	 verbose() << "]" << endmsg;
       }
     }
   
   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackReportsDecoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return Decoder::AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
    
  
// float HltTrackReportsDecoder::floatFromInt(unsigned int i)
// {
//         union IntFloat { unsigned int mInt; float mFloat; };
//         IntFloat a; a.mInt=i;
//         return a.mFloat;
// }

