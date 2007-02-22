// $Id: CaloDigitsFromRaw.cpp,v 1.9 2007-02-22 23:39:52 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "CaloDigitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitsFromRaw
//
// 2003-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CaloDigitsFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitsFromRaw::CaloDigitsFromRaw( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{  
  m_detectorNum = CaloCellCode::CaloNumFromName( name ) ;
  if ( m_detectorNum < 0 || m_detectorNum >= (int) CaloCellCode::CaloNums ) 
  { m_detectorNum = CaloCellCode::CaloNumFromName( "Ecal" ) ; }
  
  declareProperty( "Extension"  ,  m_extension = "" );
  declareProperty( "OutputType" , m_outputType = "Digits"  ) ;
  declareProperty( "PinContainer"  ,  m_pinContainerName );
  m_digitOnTES =false  ;
  m_adcOnTES =false  ;
}

//=============================================================================
// Destructor
//=============================================================================
CaloDigitsFromRaw::~CaloDigitsFromRaw() {}; 


//=========================================================================
//  Initialization 
//=========================================================================
StatusCode CaloDigitsFromRaw::initialize ( ) {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize " << name() << endmsg;
  // get DeCalorimeter
  if( 2 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    m_outputDigits   = rootOnTES() + LHCb::CaloDigitLocation::Ecal + m_extension;
    m_outputADCs = rootOnTES() + LHCb::CaloAdcLocation::Ecal   + m_extension;
    m_pinContainerName = rootOnTES() + LHCb::CaloAdcLocation::EcalPin +m_extension;
  }
  else if( 3 == m_detectorNum ) {
  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  m_outputDigits   = rootOnTES() + LHCb::CaloDigitLocation::Hcal + m_extension;
  m_outputADCs = rootOnTES() + LHCb::CaloAdcLocation::Hcal   + m_extension;
  m_pinContainerName = rootOnTES() + LHCb::CaloAdcLocation::HcalPin + m_extension;
  }
  else if( 1 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_outputDigits   = rootOnTES() + LHCb::CaloDigitLocation::Prs + m_extension;
    m_outputADCs = rootOnTES() + LHCb::CaloAdcLocation::Prs + m_extension;
    m_pinContainerName = "None";
   }
  else if( 0 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
    m_outputDigits= rootOnTES() + LHCb::CaloDigitLocation::Spd + m_extension;
    m_outputADCs  = rootOnTES() + LHCb::CaloAdcLocation::Spd + m_extension;
    m_pinContainerName = "None";
  }
  else {
    return( Error( "Invalid detector Num =  " + m_detectorNum ) );
  }


  //
  if( 0 == m_detectorNum ){
    m_spdTool = tool<ICaloTriggerBitsFromRaw>( "CaloTriggerBitsFromRaw",  name() + "Tool",this );
  }else {
    m_energyTool = tool<ICaloEnergyFromRaw>( "CaloEnergyFromRaw",  name() + "Tool",this );
  }
  
  // 
  if( m_outputType == "Digits" || 
      m_outputType == "CaloDigits" || 
      m_outputType == "Both")m_digitOnTES = true;
  if( m_outputType == "ADCs" ||
      m_outputType == "CaloAdcs" || 
      m_outputType == "Both")m_adcOnTES = true;
  if( !m_adcOnTES && !m_digitOnTES ){
    error()<< "CaloDigitsFromRaw configured to produce ** NO ** output" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_digitOnTES )debug() <<  "CaloDigitsFromRaw will produce CaloDigits on TES" 
                            << endreq;
  if( m_adcOnTES )debug() <<  "CaloDigitsFromRaw will produce CaloAdcs on TES" 
                          << endreq;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitsFromRaw::execute() {

  debug() << "==> Execute" << endreq;

  if       ( 0 == m_detectorNum ) {
    if(m_digitOnTES)convertSpd ( m_outputDigits , 3.2 * Gaudi::Units::MeV );
    if(m_adcOnTES)convertSpd ( m_outputADCs , 0. );
  } else {
    if(m_digitOnTES)convertCaloEnergies ( m_outputDigits );
    if(m_adcOnTES  )convertCaloEnergies ( m_outputADCs );
  }
  return StatusCode::SUCCESS;
};

//=========================================================================
//  Convert the SPD trigger bits to CaloDigits
//=========================================================================
void CaloDigitsFromRaw::convertSpd ( std::string containerName,
                                     double energyScale ) {

  LHCb::Calo::FiredCells spdCells = m_spdTool->spdCells( );

  if(m_digitOnTES){
    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, containerName );
    for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
          spdCells.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = new LHCb::CaloDigit( *itD, energyScale );
      digits->insert( dig );
    }
    std::stable_sort ( digits->begin(), digits->end(), 
                       CaloDigitsFromRaw::IncreasingByCellID() );
    debug() << containerName << " CaloDigit container size " << digits->size() << endreq;
  }

  if(m_adcOnTES){
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs , containerName );
    for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
          spdCells.end() != itD; ++itD ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( *itD, 1 );
      adcs->insert( adc );
    }
    debug() << containerName << " CaloAdc container size " << adcs->size() << endreq;
  }

}

//=========================================================================
//  Converts the standard calorimeter adc-energy
//=========================================================================
void CaloDigitsFromRaw::convertCaloEnergies ( std::string containerName ) {


  if(m_digitOnTES){

    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, containerName );
    std::vector<LHCb::CaloDigit>& allDigits = m_energyTool->digits( );

    for ( std::vector<LHCb::CaloDigit>::const_iterator itD = allDigits.begin();
          allDigits.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = (*itD).clone();
      digits->insert( dig );
      debug() << "ID " << dig->cellID() << " energy " << dig->e() << endreq;
    }
    std::stable_sort ( digits->begin(), digits->end(), 
                       CaloDigitsFromRaw::IncreasingByCellID() );
    debug() << containerName << " CaloDigit container size " << digits->size() << endreq;

  }
  
  if(m_adcOnTES){
    // Channel ADC
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs , containerName ); 
    std::vector<LHCb::CaloAdc>& allAdcs = m_energyTool->adcs( );
    for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allAdcs.begin();
          allAdcs.end() != itA; ++itA ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'
      adcs->insert(adc);
      debug() << "ID " << adc->cellID() << " ADC value " << adc->adc() << endreq;
    }
    debug() << " CaloAdc container "  << containerName  << "size = " << adcs->size() << endreq;


    // PinDiode ADC in a different container
    // MUST BE AFTER STANDARD ADCs
    std::vector<LHCb::CaloAdc>& allPinAdcs = m_energyTool->pinAdcs( );
    if( "None" != m_pinContainerName && 0 !=allPinAdcs.size() ){
      LHCb::CaloAdcs* pinAdcs = new LHCb::CaloAdcs();
      put(pinAdcs , m_pinContainerName);
      for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allPinAdcs.begin();
            allPinAdcs.end() != itA; ++itA ) {
        LHCb::CaloAdc* pinAdc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'
        pinAdcs->insert( pinAdc );
        debug() << "Pin-diode : ID " << pinAdc->cellID() << " ADC value " << pinAdc->adc() << endreq;
      }
      debug() << " PIN-Diode CaloAdc container " << m_pinContainerName 
              << " size = " << pinAdcs->size() << endreq;
    }
  }
}
//=============================================================================
