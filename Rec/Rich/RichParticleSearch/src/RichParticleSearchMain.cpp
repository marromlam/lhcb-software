
// $Id: $
// Include files

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// RichKernel
#include "RichKernel/RichHPDIdentifier.h"
// local
#include "RichParticleSearchMain.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/IOnOffline.h"
#include "Event/RecVertex.h"
#include "GaudiAlg/GaudiTool.h"

//namespaces

using namespace Rich::Rec::MC;


using std::vector;
//-----------------------------------------------------------------------------
// Implementation file for class : RichParticleSearchMain
//
// 2010-09-22 : Matt Coombes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichParticleSearchMain )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  RichParticleSearchMain::RichParticleSearchMain( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
    : TupleAlgBase ( name , pSvcLocator ),
      m_trSelector        ( NULL ),
      m_richRecMCTruth    ( NULL ),
      m_richPartProp      ( NULL ),
      m_ckAngle           ( NULL ),
      m_isoTrack          ( NULL ),
      m_tkIndex			  ( NULL )
{

  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks", m_maxUsedTracks = 200 );
  declareProperty( "UseMCTruth",    m_useMCTruth   = false );
  declareProperty( "Radiator",   m_radTemp = 2 ); // default is Rich2
  declareProperty( "HistoOutputLevel", m_histoOutputLevel = 0 );
  //declareProperty( "ParticleType",     m_particleType = 2 ); // pion
  //               (Aerogel) (R1Gas) (R2Gas)
  declareProperty( "MinIsolationCut",     m_IsoCut       = boost::assign::list_of (150) (100) (100), "IsolationCuts for each Radiator in mm");
  declareProperty( "MaxBeta",       m_maxBetaCut   = boost::assign::list_of (0.99985)(0.99985)(0.99985),"beta cut");

  //Cut on Histograms and NTuples
  declareProperty( "MassDevCut",    m_massDevCut = boost::assign::list_of (0.6) (0.6) (0.6),"MassDevCut");
  declareProperty( "CKDevCut",    m_CKDevCut = boost::assign::list_of (0.5) (0.5) (0.5), "CK Standard Deviation Cut" );
  declareProperty( "MinCK",     m_minCK = boost::assign::list_of (0.054)(0.0)(0.0), "Minimum CK agnle for each radiator");
  declareProperty("PlotPhotons",    m_plotPerPhoton =false, "Enter second photon loop");
  //Use Muon Information
  declareProperty("UseMuonInfo",   m_useMuonInfo = false, "Use Muon PID information");
}
//=============================================================================
// Destructor
//=============================================================================
RichParticleSearchMain::~RichParticleSearchMain() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichParticleSearchMain::initialize() {
  StatusCode sc = TupleAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  acquireTool( "RichCherenkovAngle",   m_ckAngle  );
  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichIsolatedTrack", m_isoTrack     );
  acquireTool( "RichTrackEffectiveRefIndex", m_tkIndex   );


  if ( m_useMCTruth ) {
    debug()<<"Use MC in ParticleSearch"<<endmsg;
    acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
    acquireTool( "RichParticleProperties", m_richPartProp );
  }

  m_MuonInformation = tool<IMuonInformation> ("MuonInformation");
  EvtNum =0;
  m_tkTotal = 0;

  debug()<<"IsoCuts "<<m_IsoCut<<endmsg;

  debug()<<"MaxRichRecTracks "<<m_maxUsedTracks<<endmsg;

  m_radiator = static_cast<Rich::RadiatorType>(m_radTemp);
  debug() << "Radiator:" << m_radiator << " " << m_radTemp << endmsg;

 // m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
  //debug() << "Fixed particle type:" << m_pType << endmsg;


  const std::string RAD = Rich::text(m_radiator);

  if (m_useMuonInfo){
	  StatusCode scM = m_MuonInformation->initialize();
  }

  debug() << "Finished Initialization" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichParticleSearchMain::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If any containers are empty, form them
  if ( richTracks()->empty() ) {
    if ( trackCreator()->newTracks().isFailure() )
    {
      return Error( "Problem Making Tracks" );
    }
    debug () << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endmsg;
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " Found " << richTracks()->size() << " tracks" << endmsg;
  }
  if ( (int)richTracks()->size() > m_maxUsedTracks )
  {
    debug() << "Found " << richTracks()->size() << ">"
            << m_maxUsedTracks << " max usable tracks, stopping." << endmsg;
    return StatusCode::SUCCESS;
  }

  if ( richPixels()->empty() ) {
    if ( pixelCreator()->newPixels().isFailure() )
    {
      return Error( "Problem Making Pixels" );
    }
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  if ( richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons().ignore();
    debug() << "No photons found : Created "
            << richPhotons()->size() << " RichRecPhotons" << endmsg;
  }
  int trackCounter =0;
  debug()<< "Event "<<EvtNum<<endmsg;

  //Get int for RICH; 0 Aerogel; 1 RICH1Gas; 2 Rich2Gas
  const int RICHint = GetRichInt();

  EvtNum++;
  // =========================================================================//
  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    LHCb::RichRecSegment* segment = *iSeg;
    debug()<< "Event "<<EvtNum<<endmsg;
    debug()<< "trackCounter "<<trackCounter<<endmsg;


    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    if ( m_radiator != rad ) continue;

    const std::string RAD = Rich::text(rad);

    trackCounter++;

    m_tkTotal++;

    //  double thetaExpTrue(0.0);
    int MCtype(-1);

    if ( m_useMCTruth ) {
   //   debug()<< "trackCounter "<<trackCounter<<endmsg;
      // Get true beta from true particle type

      const ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
      debug()<< "mcType: "<<mcType<<endmsg;

      //assign nmber to each type of true particle to store in NTuple
      MCtype = (int) mcType;
    }//end if use truth

    double pt=0.0;
    double momentum = std::sqrt(segment->trackSegment().bestMomentum().Mag2());
 
    int numberOfUsedTracks = 0;

    const LHCb::RichRecTrack * richtrack = segment->richRecTrack();
    const LHCb::Track* track = dynamic_cast<const LHCb::Track*>(richtrack->parentTrack());

    const double trChi2 = track->chi2();

    const int TrackType = (int) track->type();

    // T Hampson - added this line
    numberOfUsedTracks=(int)richTracks()->size();
    const LHCb::RichTrackSegment & trackSegment = segment->trackSegment();
    pt = std::sqrt( std::pow(trackSegment.bestMomentum().x(),2) +
                    std::pow(trackSegment.bestMomentum().y(),2) );



    // Get refractive index of radiator required for mass calculations
    // double refIndx = m_refractiveIndex->refractiveIndex(rad);
    double refIndx = m_tkIndex->refractiveIndex(segment);



    // Separation or isolation of tracks
    double MinimumTrackSeperation = this->MinimumTrackSeparation(segment);
    if (MinimumTrackSeperation>m_IsoCut[RICHint]){

      // Loop over photons to make photon Cut of maximum number of photons per track etc
      // ============================================================================================//
      int PhotonCounter =0;
      int TruePhotonCounter =0;
      double thetaRecSum = 0.0;
      std::vector<double> CKTheta;
      std::vector<double> trackMass;
      //double trackMassSum = 0.0;
      for ( LHCb::RichRecSegment::Photons::const_iterator iPhot2 = segment->richRecPhotons().begin();
            iPhot2 != segment->richRecPhotons().end(); ++iPhot2 )
      {
        LHCb::RichRecPhoton* photon2 = *iPhot2;
        const LHCb::RichGeomPhoton & gPhoton2 = photon2->geomPhoton();
        const double thetaRec2 = gPhoton2.CherenkovTheta();
        double beta2 = 1.0/(std::cos(thetaRec2)*refIndx);
        if (beta2 <= m_maxBetaCut[RICHint] && beta2 >=1.0/refIndx && thetaRec2 > m_minCK[RICHint]){
          PhotonCounter++;
          CKTheta.push_back(thetaRec2);
          thetaRecSum = thetaRecSum+thetaRec2;
          /* ignore this for now.
             double tkMass = momentum * std::sqrt((1/(std::pow(beta2,2)))-1.0);
             trackMass.push_back(tkMass);
             trackMassSum = trackMassSum + tkMass;
          */
        }

        bool trueParent( false );
        if ( m_useMCTruth ) {
          trueParent = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon2 ) );
        }
        if (trueParent){
          TruePhotonCounter++;
        }

        if (PhotonCounter>150){
        //  info()<<"More than 150 photons"<<endmsg;
          break;
        } //END photon cut
      }// end photon counter loop


      if (PhotonCounter>0 && PhotonCounter<149){
        double AvThetaRec = thetaRecSum/PhotonCounter;
        if (AvThetaRec > m_minCK[RICHint]){
        	double CKVariance = 0.0;
        	double thetaRecSumRemoval =0.0;

        	for (vector<double>::iterator it=CKTheta.begin(); it < CKTheta.end(); ++it){
        		CKVariance = CKVariance + std::pow((*it- AvThetaRec),2);//Standard deviation squared of CK Theta
        	}// end iterator


        	double avBetaTk = 1.0/(std::cos(AvThetaRec)*refIndx);
        	//    avTrackMass = trackMassSum/PhotonCounter;
        	double avTrackMass = momentum * std::sqrt((1/(std::pow(avBetaTk,2)))-1.0);
        	double stdDevCK = std::pow(CKVariance/(PhotonCounter-1), 0.5);

        	if (((AvThetaRec-stdDevCK)/AvThetaRec)>m_CKDevCut[RICHint] ){
            /*
              vector<double>::iterator it2;
              double stdDevMassSq = 0.0;
              for (it2=trackMass.begin(); it2 < trackMass.end(); it2++){
              stdDevMassSq = stdDevMassSq + std::pow((*it2- avTrackMass),2);//Standard deviation squared of CK Theta
              }
              stdDevMass = std::pow(stdDevMassSq/(PhotonCounter-1), 0.5);
              //  double avBetaTk = 1.0/(cos(AvThetaRec)*refIndx);

              // if (((avTrackMass-stdDevMass)/avTrackMass)>m_massDevCut[RICHint]){
              */

            if (1==1){
              //Cuts which Can be set in options file
              if ( m_histoOutputLevel > 1 ){

                //Make Plots per track
                plot1D( avTrackMass, "TrackMass", "Mass per Track",9000, 0, 45000);
                // Plot inverse Mass per Track
                plot1D( 1/avTrackMass, "InverseMass", "Inverse Mass per track", 8000, 0, 0.0008);
              }

              //Get Any Muon Information
              int hasMuonPID = 0;
              if (m_useMuonInfo){

                //Loop over proto particles
                //StatusCode scM = m_MuonInformation->initialize();
                hasMuonPID = m_MuonInformation->HasMuonInformation(track);
              }

              if ( produceNTuples())
              {
                Tuple TrackTuple = nTuple( "TrackTuple", "Rich Alignment" );
                TrackTuple->column( "avThetaRec"      ,    AvThetaRec     );
                TrackTuple->column( "tkNum",trackCounter);
                TrackTuple->column( "pt"         , pt             );
                TrackTuple->column( "refIndx"       , refIndx        );
                TrackTuple->column( "momentum"      , std::sqrt(segment->trackSegment().bestMomentum().Mag2()));
                TrackTuple->column( "nTracks"       ,  numberOfUsedTracks  );
                TrackTuple->column( "MinimumTrackSeperation" , MinimumTrackSeperation );
                TrackTuple->column( "EvtNum", EvtNum);
                TrackTuple->column( "PhotonsPerTrack", PhotonCounter);
                TrackTuple->column( "averageTrackMass", avTrackMass);
                //TrackTuple->column( "stdDevMass", stdDevMass);
                TrackTuple->column( "stdDevCK", stdDevCK);
                TrackTuple->column( "TrackType", TrackType);
                if ( m_useMCTruth ) {
                  TrackTuple->column( "MCParticleType", MCtype);
                  TrackTuple->column( "TruePhotonsPerTrack", TruePhotonCounter);
                }
                TrackTuple->column( "trChi2", trChi2);
                if(m_useMuonInfo){
                  TrackTuple->column( "hasMuonPID"    , hasMuonPID   );
                }
                TrackTuple->write();
              }//End produce NTuple

              //============================================================================================//
              if (m_plotPerPhoton){

                for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
                      iPhot != segment->richRecPhotons().end(); ++iPhot )
                {
                  LHCb::RichRecPhoton* photon = *iPhot;

                  // get the geometrical photon
                  const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

                  // Cherenkov angles
                  const double thetaRec = gPhoton.CherenkovTheta();

                  bool trueParent2( false );
                  if ( m_useMCTruth ) {
                    trueParent2 = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon ) );
                  }
                  //track Beta = v/c
                  double beta = 1.0/(cos(thetaRec)*refIndx);
                  // Mass from photon track pair
                  double mass = momentum * std::sqrt((1/(std::pow(beta,2)))-1.0);

                  if (beta <= m_maxBetaCut[RICHint] && beta >=1.0/refIndx && thetaRec > m_minCK[RICHint]){

                    if ( m_histoOutputLevel > 1 ){

                      //Make Plots per track
                      plot1D( avTrackMass, "MassperPhoton", "Mass per Photon",90000, 0, 45000);
                      plot1D( 1/avTrackMass, "InverseMassPerPhoton", "Inverse Mass per photon", 8000, 0, 0.0008);
                    }
                    if ( produceNTuples())
                    {
                      Tuple photonTuple = nTuple( "massTuplePhoton", "Rich Alignment" );
                      photonTuple->column( "CKangle"      , thetaRec        );
                      photonTuple->column( "tkNum",trackCounter);
                      photonTuple->column( "mass"          , mass           );
                      photonTuple->column( "TrueParent", trueParent2 );
                      photonTuple->column( "EvtNum", EvtNum);


                      /*
                        if (m_useMCTruth){
                        photonTuple->column( "MCParticleType", MCtype);
                        }
                      */
                      photonTuple->write();
                    }
                  }
                }//end photon loop
              }//End if m_plotPhotons
            }//End MAss Dev Cuts
          }//End CKDev Cut
        }// end Average CK cut
      }// end cut on photons

    }// End isolation Cuts

  }//end loop on track segements



  return StatusCode::SUCCESS;
}

//============================================================================


double RichParticleSearchMain::MinimumTrackSeparation(LHCb::RichRecSegment* segment){

  // Minimum track separation initialised as large value
  double MinimumTrackSeperation = 999999999.0;

  // Radiator info
  const Rich::RadiatorType rad = segment->trackSegment().radiator();

  // Hit point of current segment on HPD plane in local coords
  const Gaudi::XYZPoint & tkPtLocal = segment->LHCb::RichRecSegment::pdPanelHitPointLocal();

  //Loop over second Segments
  for ( LHCb::RichRecSegments::const_iterator iSeg2 = richSegments()->begin();
        iSeg2 != richSegments()->end(); ++iSeg2 ) {

    LHCb::RichRecSegment* segment2 = *iSeg2;

    if ( segment != segment2 ) { // check not comparing the segment with itself
      if ( segment2->trackSegment().radiator() == rad ){ // radiator for segment2 == radiator for segment
        // track hit centre of all other tracks (segment2) on HPD plane
        const Gaudi::XYZPoint & tkPtLocal2 = segment2->LHCb::RichRecSegment::pdPanelHitPointLocal();

        // Compare segment hit point to all other (segment2) hit points
        const double Xdifsq      = std::pow( fabs(tkPtLocal.x()-tkPtLocal2.x()), 2 );
        const double Ydifsq      = std::pow( fabs(tkPtLocal.y()-tkPtLocal2.y()), 2 );
        const double Zdifsq      = std::pow( fabs(tkPtLocal.z()-tkPtLocal2.z()), 2 );
        const double centreXYdif = std::sqrt(Xdifsq+Ydifsq+Zdifsq);

        //Change minimum separation if track separation is less than previous minimum
        if ( centreXYdif < MinimumTrackSeperation ){
          MinimumTrackSeperation = centreXYdif;
        }

      }  // end segment comparison loop
    } // End segment != segment2
  }
  return MinimumTrackSeperation;
}

