// $Id: CherenkovG4EventAction.cpp,v 1.17 2009-07-03 11:59:49 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "CherenkovG4EventAction.h"
#include "GaussCherenkov/CkvG4Hit.h"
#include "GaussRICH/RichG4Counters.h"
#include "CkvG4EventHitCount.h"
#include "GaussRICH/RichG4QwAnalysis.h"

// GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"

// Gaudi
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4EventAction
//
// 2002-08-19 : Sajan Easo
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CherenkovG4EventAction )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4EventAction::CherenkovG4EventAction( const std::string& type   ,
                                      const std::string& name   ,
                                      const IInterface*  parent )
  : GiGaEventActionBase( type , name , parent ),
    m_NumRichColl(0),
    m_NumRichClassicColl(0),
    m_RichHitCName(0),
    m_RichG4HistoFillSet1(0),
    m_RichG4HistoFillSet2(0),
    m_RichG4HistoFillSet3(0),
    m_RichG4HistoFillSet4(0),
    m_CherenkovG4HistoFillSet5(0),    
    m_RichG4HistoFillTimer(0),
    m_RichG4EventHitCounter(0),
    m_RichG4HitRecon(0),
    m_RichG4InputMon(0),
    m_RichEventActionVerboseLevel(0),
    m_RichEventActionHistoFillActivateSet1(false),
    m_RichEventActionHistoFillActivateSet2(false),
    m_RichEventActionHistoFillActivateSet3(false),
    m_RichEventActionHistoFillActivateSet4(false),
    m_RichEventActionHistoFillActivateSet5(false),
    m_RichEventActionHistoFillActivateTimer(false),
    m_RichG4EventHitActivateCount(false),
    m_RichG4EventActivateCkvRecon(false),
    m_RichG4HistoActivateQw(false),
    m_RichG4HitReconUseSatHit(true),
    m_RichG4HitReconUseStdRadHit(true),
    m_RichG4HitReconUseMidRadiator(false),
    m_RichG4InputMonActivate(false),
    m_IsRichG4FirstEvent(true),
    m_CkvG4HitReconUseOnlySignalHit(false),
    m_CkvG4HitReconUseOnlyHighMom(false)
{
  declareProperty( "RichEventActionVerbose",
                   m_RichEventActionVerboseLevel );

  declareProperty( "RichEventActionHistoFillSet1",
                   m_RichEventActionHistoFillActivateSet1);
  declareProperty( "RichEventActionHistoFillSet2",
                   m_RichEventActionHistoFillActivateSet2);
  declareProperty( "RichEventActionHistoFillSet3",
                   m_RichEventActionHistoFillActivateSet3);
  declareProperty( "RichEventActionHistoFillSet4",
                   m_RichEventActionHistoFillActivateSet4);
  declareProperty( "RichEventActionHistoFillSet5",
                   m_RichEventActionHistoFillActivateSet5);
  declareProperty( "RichEventActionHistoFillTimer",
                   m_RichEventActionHistoFillActivateTimer);

  declareProperty("RichG4EventActivateCounting" ,
                  m_RichG4EventHitActivateCount);

  declareProperty("RichG4EventActivateCkvReconstruction",
                  m_RichG4EventActivateCkvRecon);

  declareProperty("RichG4EventHitReconUseSaturatedHit" ,
                  m_RichG4HitReconUseSatHit);

  declareProperty("RichG4EventHitReconUseStdRadiatorHit",
		  m_RichG4HitReconUseStdRadHit);
          
  declareProperty("RichG4EventHitReconUseMidRadiator",
                  m_RichG4HitReconUseMidRadiator);

  declareProperty("RichG4QuartzWindowCkvHistoActivate",
		  m_RichG4HistoActivateQw);

  declareProperty("RichG4InputMonitorActivate",
		  m_RichG4InputMonActivate);

  declareProperty("RichG4HitReconUseSignalHit",   
             m_CkvG4HitReconUseOnlySignalHit);


  declareProperty("RichG4HitReconUseHighMomTk",
                  m_CkvG4HitReconUseOnlyHighMom);

  // m_RichHitCName= new CkvG4HitCollName();
  // m_NumRichColl=m_RichHitCName->RichHCSize();

  //  m_RichG4CollectionID.reserve(m_NumRichColl);
  // for (int ic=0; ic<m_NumRichColl; ++ic) {
  //  m_RichG4CollectionID.push_back(-1);
  //  }

}

//=============================================================================
// Destructor
//=============================================================================
CherenkovG4EventAction::~CherenkovG4EventAction( ){

  delPointer( m_RichG4HistoFillSet1 );
  delPointer( m_RichG4HistoFillSet2 );
  delPointer( m_RichG4HistoFillSet3 );
  delPointer( m_RichG4HistoFillSet4 );
  delPointer( m_CherenkovG4HistoFillSet5 );
  delPointer( m_RichG4HistoFillTimer );
  delPointer( m_RichG4EventHitCounter );
  delPointer( m_RichG4InputMon);
  delPointer(m_RichHitCName );
  
}
StatusCode CherenkovG4EventAction::initialize() 
{
 StatusCode sc = GiGaEventActionBase::initialize();
  if (sc.isFailure()) return sc;

  //  if(m_RichEventActionHistoFillActivateSet1) {

  m_RichG4HistoFillSet1 = new CkvG4HistoFillSet1();

  //  }
  //   if(m_RichEventActionHistoFillActivateSet2) {
  m_RichG4HistoFillSet2 = new CherenkovG4HistoFillSet2();

  // }
  m_RichG4HistoFillSet3 = new RichG4HistoFillSet3();

  //  the following done later.
  //      m_RichG4HistoFillSet4 = new RichG4HistoFillSet4();

  // if(m_RichEventActionHistoFillActivateTimer) {

  m_RichG4HistoFillTimer = new RichG4HistoFillTimer();

  //  }

  m_RichG4EventHitCounter = new  CkvG4EventHitCount();

  // Now to initialize the reconstruction of the the
  // ckv angle for test
  m_RichG4HitRecon = new CherenkovG4HitRecon();

  // now intialise the Rich input monitors.
  
  m_RichG4InputMon = new RichG4InputMon();

  if(!m_RichHitCName)  m_RichHitCName= new CkvG4HitCollName();


  return sc;  
  
}
StatusCode CherenkovG4EventAction::finalize() 
{
 
  return GiGaEventActionBase::finalize();

 
  
}




//=============================================================================
// BeginOfEventAction (G4)
//=============================================================================
void CherenkovG4EventAction::BeginOfEventAction ( const G4Event* /* aEvt */ )
{
  MsgStream msg(msgSvc(), name());


  if(m_RichEventActionHistoFillActivateTimer) {
    m_RichG4HistoFillTimer->RichG4BeginEventTimer();
  }

  if(!m_RichHitCName)  m_RichHitCName= new CkvG4HitCollName();

  m_NumRichColl=m_RichHitCName->RichHCSize();
  m_RichG4CollectionID.assign(m_NumRichColl,-1);
  

  //  m_RichG4CollectionID.reserve(m_NumRichColl);
  //for (int ic=0; ic<m_NumRichColl; ++ic) {
  //  m_RichG4CollectionID.push_back(-1);
  //  }

  G4SDManager * SDman = G4SDManager::GetSDMpointer();
  G4String colNam;
  for (int icol=0; icol<m_NumRichColl; ++icol ) {
    if(m_RichG4CollectionID[icol]<0){
      colNam=  m_RichHitCName->RichHCName(icol);
     m_RichG4CollectionID[icol] = SDman->GetCollectionID(colNam);
      
    }
  }

  // Now to initialize the Rich Counters

  RichG4Counters* aRichCounter=RichG4Counters::getInstance();
  aRichCounter->InitRichEventCounters();
  // now monitor the material input properties to Rich
  // This is done only at the beginning of the first event.
  // This is under a switch so that by default it is not activated.

  if(m_RichG4InputMonActivate) {

    if(m_RichG4InputMon->FirstMonInstance()) {
      
      m_RichG4InputMon->MonitorRich1GasRadiatorRefIndex();
      m_RichG4InputMon->setFirstMonInstance(false);

    }
  
  }

  // now for filling the occupancies
    if(m_RichEventActionHistoFillActivateSet5) {
      m_CherenkovG4HistoFillSet5= new CherenkovG4HistoFillSet5();
      m_CherenkovG4HistoFillSet5->InitCherenkovG4HistoFillSet5();
      m_RichG4HitRecon -> setCherenkovG4HistoFillSet5Occp(  m_CherenkovG4HistoFillSet5);
      //     m_RichG4HitRecon -> setuseOnlySignalHitsInRecon(m_CkvG4HitReconUseOnlySignalHit);
      // m_RichG4HitRecon -> setactivateMinMomForTrackRecon(m_CkvG4HitReconUseOnlyHighMom);
    }
    

  // now for the reconstruction for test.


  if(m_RichG4EventActivateCkvRecon) {
      m_RichG4HitRecon -> setuseOnlySignalHitsInRecon(m_CkvG4HitReconUseOnlySignalHit);
      m_RichG4HitRecon -> setactivateMinMomForTrackRecon(m_CkvG4HitReconUseOnlyHighMom);

    m_RichG4HitRecon ->setSatHitUse( m_RichG4HitReconUseSatHit);
    m_RichG4HitRecon ->setMidRadiatorUse(m_RichG4HitReconUseMidRadiator);
    m_RichG4HitRecon ->setuseOnlyStdRadiatorHits(m_RichG4HitReconUseStdRadHit);


    if(m_RichEventActionHistoFillActivateSet4) {
      m_RichG4HistoFillSet4= new CherenkovG4HistoFillSet4();
      m_RichG4HitRecon->setRichG4HistoFillSet4Ckv( m_RichG4HistoFillSet4 );

    }

    if(   m_RichG4HitRecon->getRichG4CkvRec() == 0 ){

      m_RichG4HitRecon->setRichG4CkvRec();

    }
    if(   m_RichG4HitRecon->getRichG4ReconFlatMirr() == 0) {
      m_RichG4HitRecon->setRichG4FlatMirr();
      
    }
    if(   m_RichG4HitRecon->getRichG4ReconFlatMirr() != 0) {
    
      m_RichG4HitRecon->getRichG4CkvRec()->
        setCurReconFlatMirr(m_RichG4HitRecon->getRichG4ReconFlatMirr());
    }
    
  }


  if( m_IsRichG4FirstEvent ) {
    if( m_RichG4HistoActivateQw ) {

      RichG4QwAnalysis* aRichG4QwAnalysis = RichG4QwAnalysis::getRichG4QwAnalysisInstance();
      aRichG4QwAnalysis->InitQwAnalysis();
    }

    m_IsRichG4FirstEvent = false;
  } else {
    if( m_RichG4HistoActivateQw ) {
      RichG4QwAnalysis* aRichG4QwAnalysis = RichG4QwAnalysis::getRichG4QwAnalysisInstance();
      aRichG4QwAnalysis->reset_NumPartInQwHisto();

    }
  }



  // Print("'BeginOfEventAction' method is invoked by CherenkovG4EventAction");
}

//=============================================================================
// G4
//=============================================================================
void CherenkovG4EventAction::EndOfEventAction( const G4Event* anEvent  /* event */ )
{



  if(m_RichEventActionHistoFillActivateTimer) {

    m_RichG4HistoFillTimer->RichG4EndEventTimer();

  }

  MsgStream msg(msgSvc(), name());

  G4int CurEventNum=anEvent->GetEventID();


  if( m_RichEventActionHistoFillActivateSet1) {

    //Now for the histo ids which are filled in this class.
    m_RichG4HistoFillSet1->FillRichG4HistoSet1(anEvent,
                                        m_NumRichColl,m_RichG4CollectionID);

  }


  if( m_RichG4EventHitActivateCount) {


    m_RichG4EventHitCounter-> RichG4CountAndClassifyHits(
                                                         anEvent, m_NumRichColl,m_RichG4CollectionID);



    m_RichG4EventHitCounter-> RichG4CountSaturatedHits(
                                                       anEvent, m_NumRichColl,m_RichG4CollectionID);

    
    m_RichG4EventHitCounter-> CkvG4CountFullAcceptSatHits(
                                               anEvent, m_NumRichColl,m_RichG4CollectionID);

    


    if( m_RichEventActionVerboseLevel >= 2 ) {

      PrintRichG4HitCounters();

    }

  }


  if( m_RichEventActionHistoFillActivateSet2) {

    //Now for the histo ids which are filled in this class.
    m_RichG4HistoFillSet2->FillRichG4HistoSet2(anEvent,
                                               m_NumRichColl,m_RichG4CollectionID);

  }

  if( m_RichEventActionHistoFillActivateSet3) {

    //Now for the histo ids which are filled in this class.
    m_RichG4HistoFillSet3->FillRichG4HistoSet3( );

    //  m_RichG4HistoFillSet3->FillRichG4HistoSet3( anEvent,
    //                                           m_NumRichColl,m_RichG4CollectionID);

  }

    if(m_RichEventActionHistoFillActivateSet5) {
      m_RichG4HitRecon -> RichG4GetOccupancies(anEvent,
                              m_NumRichColl, m_RichG4CollectionID);
      
      m_CherenkovG4HistoFillSet5->FillRichG4HistoSet5NumH();
    }

  if(m_RichG4EventActivateCkvRecon) {

    if( m_RichG4HitRecon != 0 ) {
     
      m_RichG4HitRecon->RichG4ReconstructCherenkovAngle(anEvent,
                              m_NumRichColl, m_RichG4CollectionID);

    }

  }


    if( m_RichG4HistoActivateQw ) {

      RichG4QwAnalysis* aRichG4QwAnalysis = RichG4QwAnalysis::getRichG4QwAnalysisInstance();
      aRichG4QwAnalysis->WriteOutQwNtuple();
    }

  //get the trajectories
  G4TrajectoryContainer* trajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(trajectoryContainer){n_trajectories=trajectoryContainer->entries();
  msg << MSG::DEBUG << "     " << n_trajectories
      << " Tracks are stored in Trajectorycontainer in event       "
      <<CurEventNum << endreq;
  }

   G4HCofThisEvent * HCE;
 

  //get the hits by
  // looping through the the hit collections

  //  G4HCofThisEvent * HCE;
  //  G4int nHitTotRich1=0;
  // for (int ihcol=0; ihcol<m_NumRichColl; ++ihcol ) {
  //  if(m_RichG4CollectionID[ihcol] >=0 ) {
  //    HCE = anEvent->GetHCofThisEvent();
  //    CkvG4HitsCollection* RHC=NULL;
  //    if(HCE){
  //      RHC = (CkvG4HitsCollection*)(HCE->GetHC(m_RichG4CollectionID[ihcol]));
  //    }
  //    if(RHC){
  //      G4int nHitInCurColl = RHC->entries();
  //      msg << MSG::DEBUG << "EndEvAction      "<< nHitInCurColl
  //          <<"   are stored in RichHitCollection set   "<<ihcol<< endreq;
  //      if(ihcol == 0 || ihcol == 1 ) {
  //        nHitTotRich1 += nHitInCurColl;
  //
  //      }
  //
  //   }
  // }
  // }


  // The drawing of hits should be on a switch.
  // For this is switched off.
  G4bool DrawRichHits= false;
  G4bool PrintRichHits=false;
  if( m_RichEventActionVerboseLevel > 2 ) PrintRichHits=true;


  //Now loop through the hits

  if(   DrawRichHits ||   PrintRichHits ) {

    for (int ihcold=0; ihcold<m_NumRichColl; ++ihcold ) {
      msg << MSG::INFO << "Now drawing Rich hits for collection  "<<ihcold <<endreq;
      if(m_RichG4CollectionID[ihcold] >=0 ) {
        HCE = anEvent->GetHCofThisEvent();
        CkvG4HitsCollection* RHCD=NULL;
        if(HCE){
          RHCD = (CkvG4HitsCollection*)(HCE->GetHC(m_RichG4CollectionID[ihcold]));
        }
        if(RHCD){
          G4int nHitHC = RHCD->entries();
          for (G4int ih=0; ih<nHitHC; ++ih ) {
            CkvG4Hit* aHit = (*RHCD)[ih];
            //Now to draw the hits
            if( DrawRichHits) {
              aHit->DrawPEOrigin();
            }
            // Now to print the info of each hit.

            if( PrintRichHits) {

              aHit->Print();
            }


          }

        }
      }

    }
  }







  //   Print("'EndOfEventAction' method is invoked by CherenkovG4EventAction");

}

//=============================================================================
// PrintRichG4HitCounters
//=============================================================================
void CherenkovG4EventAction::PrintRichG4HitCounters()
{

  MsgStream msg(msgSvc(), name());

  RichG4Counters* aRichG4Counter =  RichG4Counters::getInstance();

  int NumTotHitRich1 = aRichG4Counter-> NumHitTotRich1All();
  int NumHitGasRich1 = aRichG4Counter-> NumHitTotRich1Gas();
  int NumHitAgelRich1= aRichG4Counter->  NumHitTotRich1Agel();
  int NumTotHitRich2 = aRichG4Counter-> NumHitTotRich2All();
  int NumTotHitNoRadiator = aRichG4Counter->NumHitTotNoRadiator();

  const std::vector<int> & NumHitSatGasRich1 =aRichG4Counter->
    NumHitSaturatedPerTrackRich1Gas();
  const std::vector<int> & NumHitSatAgelRich1 = aRichG4Counter->
    NumHitSaturatedPerTrackRich1Agel();

  const std::vector<int> & NumHitSatGasRich2 =aRichG4Counter->
    NumHitSaturatedPerTrackRich2Gas();


  int NumHitSinglePartGunPrimaryGasRich1 =
    aRichG4Counter->  NumHitPartGunPrimaryPartRich1Gas();

  int NumHitSinglePartGunPrimaryAgelRich1=
    aRichG4Counter-> NumHitPartGunPrimaryPartRich1Agel();

  int NumHitSinglePartGunPrimaryGasRich2 =
    aRichG4Counter->  NumHitPartGunPrimaryPartRich2Gas();

  msg << MSG::DEBUG <<" Rich1 Hits : Total Overall FromRich1Gas FromAgel    "
      <<  NumTotHitRich1<<"   "<<  NumHitGasRich1
      << "    "<<  NumHitAgelRich1
      <<"   TotalNumRich2Hits =  "<<NumTotHitRich2 
      << "  TotalNumHitsNoRadiator  =   "<<NumTotHitNoRadiator
      << endreq;

  for(int ihgas=0; ihgas < (int) NumHitSatGasRich1.size() ; ++ihgas ) {
    if(  NumHitSatGasRich1[ihgas]> 0 )
    {

      msg << MSG::DEBUG <<"Rich1 Hits: tklistNum SaturatedPerTrackFromGas  "
          <<ihgas <<"   "<< NumHitSatGasRich1[ihgas]
          <<endreq;
    }

  }
  for(int ihagel=0; ihagel< (int) NumHitSatAgelRich1.size() ; ++ihagel ) {
    if( NumHitSatAgelRich1[ihagel] > 0    ){
      msg << MSG::DEBUG <<"Rich1 Hits: tklistNum SaturatedPerTrackFromAgel  "
          <<ihagel <<"   "<< NumHitSatAgelRich1[ihagel]
          <<endreq;

    }


  }

  for(int ihgas2=0; ihgas2 < (int) NumHitSatGasRich2.size() ; ++ihgas2 ) {
    if(  NumHitSatGasRich2[ihgas2]> 0 )
    {

      msg << MSG::DEBUG <<"Rich2 Hits: tklistNum SaturatedPerTrackFromRich2Gas  "
          <<ihgas2 <<"   "<< NumHitSatGasRich2[ihgas2]
          <<endreq;
    }

  }


  msg << MSG::DEBUG <<"Rich1Hits: SinglePartGun FromPrimaryPartFromGas "
      <<"   FromPrimaryPartFromAerogel   "
      << NumHitSinglePartGunPrimaryGasRich1<<"     "
      << NumHitSinglePartGunPrimaryAgelRich1<<endreq;

  msg << MSG::DEBUG <<"Rich2Hits: SinglePartGun FromPrimaryPartFromRich2Gas "
      << NumHitSinglePartGunPrimaryGasRich2<<endreq;


  int aNumPhotProdRich1Gas = aRichG4Counter->NumPhotProdRich1Gas() ;
  int aNumPhotGasOnRich1Mirror1 =
    aRichG4Counter->NumPhotGasOnRich1Mirror1();
  int aNumPhotGasOnRich1Mirror2 =
    aRichG4Counter->NumPhotGasOnRich1Mirror2();
  int  aNumPhotGasOnGasQW =
    aRichG4Counter-> NumPhotGasOnRich1GasQW();
  int aNumPhotGasOnHpdQW  =
    aRichG4Counter-> NumPhotGasOnRich1HpdQW();

  int aNumPhotGasBeforeQE =
    aRichG4Counter-> NumPhotGasRich1BeforeQE();
  int aNumPhotGasAfterQE =
    aRichG4Counter->  NumPhotGasRich1AfterQE();

  int aNumPeGasSiDet = aRichG4Counter->NumPhotGasRich1SiDet();

  msg << MSG::DEBUG <<"Rich1Hits: FromC4F10 NumPhot at Prod Mirror1 Mirror2 "
      <<"  GasQW HpdQW BefQE AftQE elnSiDet "
      <<aNumPhotProdRich1Gas<<"   " <<aNumPhotGasOnRich1Mirror1
      <<"   "<<aNumPhotGasOnRich1Mirror2<<"   "
      <<aNumPhotGasOnGasQW<<"    "<< aNumPhotGasOnHpdQW<<"   "
      <<aNumPhotGasBeforeQE <<"   "<< aNumPhotGasAfterQE
      <<"  "<< aNumPeGasSiDet <<endreq;


  int aNumPhotProdRich1Agel =
    aRichG4Counter->NumPhotProdRich1Agel();

  int aNumPhotDownstrAgel =
    aRichG4Counter->NumPhotAgelAtAgelDownstrZ();

  int aNumPhotAgelOnRich1Mirror1 =
    aRichG4Counter->NumPhotAgelOnRich1Mirror1();
  int aNumPhotAgelOnRich1Mirror2 =
    aRichG4Counter->NumPhotAgelOnRich1Mirror2();

  int aNumPhotAgelOnGasQW =
    aRichG4Counter-> NumPhotAgelOnRich1GasQW();
  int aNumPhotAgelOnHpdQW  =
    aRichG4Counter-> NumPhotAgelOnRich1HpdQW();

  int aNumPhotAgelBeforeQE =
    aRichG4Counter-> NumPhotAgelRich1BeforeQE();
  int aNumPhotAgelAfterQE =
    aRichG4Counter->  NumPhotAgelRich1AfterQE();

  int aNumPeAgelSiDet =
    aRichG4Counter->NumPhotAgelRich1SiDet();


  msg << MSG::DEBUG <<"Rich1 Hits: fromAerogel NumPhot at Prod AgelDownstrZ "
      <<"   Mirror1 Mirror2 "
      <<"   GasQW HpdQW BefQE AftQE elnSiDet "
      <<aNumPhotProdRich1Agel<<"    "
      <<  aNumPhotDownstrAgel<<"    "
      <<aNumPhotAgelOnRich1Mirror1
      <<"   "<<aNumPhotAgelOnRich1Mirror2<<"   "
      <<aNumPhotAgelOnGasQW<<"    "<< aNumPhotAgelOnHpdQW<<"   "
      <<aNumPhotAgelBeforeQE <<"   "<< aNumPhotAgelAfterQE
      <<"  "<< aNumPeAgelSiDet <<endreq;


  //    cout<<"End of PrintRich G4Hits "<<endl;

}

//=============================================================================





