// $Id: L0DUConfigProvider.cpp,v 1.15 2010-01-20 16:30:58 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "L0DUConfigProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUConfigProvider
//
//  Provide a **SINGLE** L0DU configuration according to 
//  the Trigger Configuration Key
//  Configuration described in options file
//
//
// 2007-10-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUConfigProvider );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUConfigProvider::L0DUConfigProvider( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_data(),
    m_conditions(),
    m_channels(),
    m_configs(),
    m_cData(0),
    m_pData(0),
    m_template(false){
  declareInterface<IL0DUConfigProvider>(this);
  
  declareProperty( "constData"               , m_constData);
  declareProperty( "Data"                    , m_data     );
  declareProperty( "Conditions"              , m_conditions );
  declareProperty( "Channels"                , m_channels );
  declareProperty( "Triggers"                , m_triggers );
  // for options defined configuration
  declareProperty( "Description"             , m_def     = "NO DESCRIPTION");  
  declareProperty( "Name"                    , m_recipe  = "");  
  declareProperty( "Separators"              , m_sepMap);
  declareProperty( "FullDescription"         , m_detail  = false);


  // TCK from name

  int idx = name.find_last_of(".")+1;
  std::string nam = name.substr(idx,std::string::npos);
  if(nam == LHCb::L0DUTemplateConfig::Name )m_template = true;  

  if( m_template ){
    m_tckopts = LHCb::L0DUTemplateConfig::TCKValue;
  }else{
    int index = name.rfind("0x") + 2 ;
    std::string tck = name.substr( index, name.length() );
    std::istringstream is( tck.c_str() );
    is >> std::hex >> m_tckopts  ;
  }
  

  //
  m_sepMap["["] = "]";
  m_dataFlags.push_back("name");
  m_dataFlags.push_back("data");
  m_dataFlags.push_back("operator");

  m_condFlags.push_back("name");
  m_condFlags.push_back("data");
  m_condFlags.push_back("comparator");
  m_condFlags.push_back("threshold");
  m_condFlags.push_back("index");
  
  
  m_chanFlags.push_back("name");
  m_chanFlags.push_back("condition");
  m_chanFlags.push_back("rate");
  m_chanFlags.push_back("enable");
  m_chanFlags.push_back("disable");
  m_chanFlags.push_back("mask");
  m_chanFlags.push_back("index");

  m_trigFlags.push_back("name");
  m_trigFlags.push_back("channel");  
  m_trigFlags.push_back("index");  
  m_trigFlags.push_back("type");

  // define the allowed operator and comparators
  m_comparators.push_back(">");
  m_comparators.push_back("<");
  m_comparators.push_back("==");
  m_comparators.push_back("!=");
  //
  m_operators.push_back(std::make_pair("Id",1));  // pair(operator,dimension)
  m_operators.push_back(std::make_pair("+",2));
  m_operators.push_back(std::make_pair("-",2));
  m_operators.push_back(std::make_pair("&",2));
  m_operators.push_back(std::make_pair("^",2));
  // index of the predefined triggers
  m_tIndices["L0Ecal" ] = 0 ;
  m_tIndices["L0Hcal" ] = 1 ;
  m_tIndices["L0Muon" ] = 2 ;
  m_tIndices["Other"  ] = 3 ;
}
//============================================================================= 
// Destructor 
//=============================================================================
L0DUConfigProvider::~L0DUConfigProvider() {}  


//============
// finalize 
//============
StatusCode L0DUConfigProvider::finalize(){

  debug() << "release L0DUConfigProvider" << endmsg;
  
  
  debug() << "Deleting " <<  m_triggersMap.size() << " L0DUTrigger* " << endmsg;
  for(LHCb::L0DUTrigger::Map::iterator id=m_triggersMap.begin();id!=m_triggersMap.end();id++){
   delete (*id).second;
  }
  
  debug() << "Deleting " <<  m_dataMap.size() << " L0DUElementaryData* " << endmsg;
  for(LHCb::L0DUElementaryData::Map::iterator id=m_dataMap.begin();id!=m_dataMap.end();id++){
    delete (*id).second;
  }  
  debug() << "Deleting " <<  m_conditionsMap.size() << " L0DUElementaryConditions* " << endmsg;
  for(LHCb::L0DUElementaryCondition::Map::iterator id=m_conditionsMap.begin();id!=m_conditionsMap.end();id++){
    delete (*id).second;
  }
  
  debug() << "Deleting " <<  m_channelsMap.size() << " L0DUElementaryChannels* " << endmsg;
  for(LHCb::L0DUChannel::Map::iterator id=m_channelsMap.begin();id!=m_channelsMap.end();id++){
   delete (*id).second;
  }


  //delete m_config;
  for(std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.begin();it!=m_configs.end();++it){
    LHCb::L0DUConfigs* configs = (*it).second;
    if( configs == NULL )continue;
    configs->release();
    delete configs;
  }
  m_configs.clear();
  return GaudiTool::finalize();
}

//============================================================================= 
// Initialize is the main method
//=============================================================================
StatusCode L0DUConfigProvider::initialize(){
  debug() << "Initialize L0DUConfigProvider" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  //get condDB tool
  m_condDB = tool<IL0CondDBProvider>("L0CondDBProvider");

  //  
  m_separators = *(m_sepMap.begin());
  if(m_sepMap.size() != 1)
    warning() << "A single pair of separators must be defined - will use the first : " 
              << m_separators.first << "data" << m_separators.second << endmsg;

  std::string slot = ( "" == rootInTES() ) ? "T0" : rootInTES() ;



  // load predefined elementary data
  hardcodedData();
  constantData();
  //=====================================
  if(m_def == "")m_def = "No Description";
  if(m_template)m_def += " (L0DUConfig.Template)";
  else if(m_channels.size()   == 0  || m_conditions.size() == 0) {
    Warning("Configuration (TCK = " + format("0x%04X" , m_tckopts )  + ") is empty",StatusCode::SUCCESS).ignore();
  }
  if( slot == "") slot = "T0";

  sc = createTriggers();  // the main method - crate triggers->channels->conditions(->data) chain

  if(sc.isFailure()){
    fatal() << " configuring L0DU (TCK = " << format("0x%04X" , m_tckopts )  << ") failed" << endmsg;
    return StatusCode::FAILURE; 
  }

  createConfig(slot);
  return StatusCode::SUCCESS;
}


//-------------------------------------------
void L0DUConfigProvider::createConfig(std::string slot){

  
  // create L0DU configuration 
  m_config = new LHCb::L0DUConfig(m_tckopts);

  m_config->setDefinition( m_def);

  if(m_recipe == ""){
    std::stringstream recipe("");
    recipe << "Undefined recipe name for TCK=" <<  format("0x%04X" , m_tckopts ) ;    
    m_config->setRecipe( recipe.str() );
  }else{
    m_config->setRecipe( m_recipe );
  }  
  //=====================================
  m_config->setData( m_dataMap );
  m_config->setConditions( m_conditionsMap );
  m_config->setChannels( m_channelsMap );
  if(m_triggersMap.size() !=0)m_config->setTriggers( m_triggersMap);
  std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.find( slot );
  if(it == m_configs.end()){
    LHCb::L0DUConfigs* confs = new LHCb::L0DUConfigs();
    m_configs[slot] = confs;
  }
  m_configs[slot]->insert( m_config );
  //=====================================
  printConfig(*m_config,slot);
}



void L0DUConfigProvider::printConfig(LHCb::L0DUConfig config,std::string slot){
  if( slot == "") slot = "T0";
  info() <<  "-------------------------------------------------------------"<<endmsg;
  info() << "**** L0DU Config loading : L0TCK = " << format("0x%04X" , config.tck()) << " for slot " 
         << slot << " ==> OK " << endmsg;
  debug() << "              - " << config.data().size()<< " data with "<<endmsg;
  debug() << "                    - " << m_pData << " predefined data "<<endmsg;
  debug() << "                    - " << m_cData << " constant   data "<<endmsg;
  debug() << "                    - " << config.data().size()-m_cData-m_pData << " new data "<<endmsg;
  debug() << "              - " << config.conditions().size() << " Elementary conditions " << endmsg;
  debug() << "              - " << config.channels().size()   << " Channels " << endmsg;
  debug() << " " << endmsg;
  info() <<  "Short description :: " << config.definition()  << endmsg;
  if(m_detail)info() << "Full description  :: " << config.description() << endmsg;
}



//-----------------
// Predefined data 
//-----------------
//=============================================================================
void L0DUConfigProvider::hardcodedData( ) {
  using namespace L0DUBase::Conditions;  
  
  // 1- content fields (Et/Pt/Multiplicity) ...
  predefinedData("Electron(Et)"       , Electron  );
  predefinedData("Photon(Et)"         , Photon    );    
  predefinedData("Hadron(Et)"         , Hadron    );  
  predefinedData("LocalPi0(Et)"       , LocalPi0  );
  predefinedData("GlobalPi0(Et)"      , GlobalPi0 );
  predefinedData("Sum(Et)"            , SumEt     );
  predefinedData("Spd(Mult)"          , SpdMult   );
  predefinedData("Muon1(Pt)"          , Muon1     );
  predefinedData("Muon2(Pt)"          , Muon2     );
  predefinedData("Muon3(Pt)"          , Muon3     );
  predefinedData("DiMuon(Pt)"         , DiMuon    );
  predefinedData("PUHits(Mult)"       , PuHits    );
  predefinedData("PUPeak1(Cont)"      , PuPeak1   );
  predefinedData("PUPeak2(Cont)"      , PuPeak2   );
  // 2- addresses Fields
  predefinedData("Electron(Add)"      , CaloAddress );
  predefinedData("Photon(Add)"        , CaloAddress );
  predefinedData("Hadron(Add)"        , CaloAddress );
  predefinedData("LocalPi0(Add)"      , CaloAddress );
  predefinedData("GlobalPi0(Add)"     , CaloAddress );
  predefinedData("Muon1(Add)"         , MuonAddress );
  predefinedData("Muon2(Add)"         , MuonAddress );
  predefinedData("Muon3(Add)"         , MuonAddress );
  predefinedData("PUPeak1(Add)"       , PuPeak1Pos  );
  predefinedData("PUPeak2(Add)"       , PuPeak2Pos  );
  // 3 - additional elementary data field (muon sign ... )
  predefinedData("Muon1(Sgn)"         , MuonSign );
  predefinedData("Muon2(Sgn)"         , MuonSign );
  predefinedData("Muon3(Sgn)"         , MuonSign );  
}

//=============================================================================
void L0DUConfigProvider::predefinedData(const std::string& name,const int param[L0DUBase::Conditions::LastIndex]){
  LHCb::L0DUElementaryData* data = 
    new LHCb::L0DUElementaryData(m_pData,LHCb::L0DUElementaryData::Predefined,name,"Id",name);
  m_conditionOrder[name] = param[L0DUBase::Conditions::Order];
  m_dataMap[name]=data ;
  debug() << "Predefined Data : " << data->description() << endmsg;
  m_pData++;
}

//---------------
// Constant data 
//---------------

void L0DUConfigProvider::constantData(){
  for(std::map<std::string,int>::const_iterator idata = m_constData.begin();idata!=m_constData.end();++idata){
    LHCb::L0DUElementaryData* data = 
      new LHCb::L0DUElementaryData(m_pData+m_cData,LHCb::L0DUElementaryData::Constant,(*idata).first,"Id",(*idata).first);
    data->setOperand( (*idata).second , 1. );
    m_dataMap[(*idata).first]=data ;
    debug() << "Constant Data : " << data->summary() << endmsg;
    m_cData++;
  }  
}


//===============================================================
std::vector<std::string> L0DUConfigProvider::Parse(std::string flag, std::vector<std::string> config ){

  std::vector<std::string> values;
  for(std::vector<std::string>::iterator iconfig= config.begin(); iconfig != config.end(); iconfig++){
    std::string val;
    // produce case-insensitive result
    std::string conf(*iconfig);
    std::string uConf(conf);
    std::string uFlag(flag);
    std::transform( conf.begin() , conf.end() , uConf.begin () , ::toupper ) ;
    std::transform( flag.begin() , flag.end() , uFlag.begin () , ::toupper ) ;
    int index = uConf.find( uFlag );
    if(index != -1 ){
      verbose() << "Flag '" << flag << "' found in the data string '" << *iconfig << "'" << endmsg;

      // loop over separators
      int id = (*iconfig).find(m_separators.first);
      if( id < 0){
        error() << "Unable to parse the tag " << *iconfig 
                << " due to a missing separator (" <<m_separators.first<<"..."<<m_separators.second<<")"<< endmsg;
        values.clear();
        return values;
      }
      while(id >= 0){
        int from = (*iconfig).find(m_separators.first,id); 
        int to   = (*iconfig).find(m_separators.second,from+1);
        if(from != -1 && to != -1){
          val = (*iconfig).substr(from+1, to-from-1);
          verbose() << "parsed value = '" << val << "'" <<endmsg;  
          values.push_back( val );
          id = (*iconfig).find(m_separators.first,to+1);
        }
        else{
          id = -1;    
          error() << "Unable to parse the tag " << *iconfig 
                  << " due to a missing separator (" <<m_separators.first<<"..."<<m_separators.second<<")"<< endmsg;
          values.clear();
          return values;
        }
        
      }
    }
  }
  return values;
}

  



//===============================================================
StatusCode L0DUConfigProvider::createData(){

  int id = m_dataMap.size();
  for(ConfigIterator iconfig = m_data.begin(); iconfig != m_data.end() ; ++iconfig){
  std::vector<std::string> values;

    // check all tags exist
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_dataFlags.begin();iflag!=m_dataFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new DATA defined via options (num =  " 
                << iconfig-m_data.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new data description are : " << m_dataFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }



    // The Data name 
    //--------------
    values = Parse("name", *iconfig);

    if(values.size() != 1){
      error() << "The DATA definied via option (num = " << iconfig-m_data.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXXX.Data +={ {''name=[DataName]'', "
             <<" ''operator=[+/-/&/^]'', ''data=[OperandName1]'', ''data=[OperandName2]'' , ...}};" << endmsg;

      return StatusCode::FAILURE;  
    }
    std::string dataName = *(values.begin()); // The NAME

    // Check the data already exist or not 
    LHCb::L0DUElementaryData::Map::iterator it = m_dataMap.find( dataName );
    if( it != m_dataMap.end() ){
      warning() << "One L0DUElementaryData  with name : '" << dataName 
                <<"' already exists - Please check your settings" << endmsg;  
      return StatusCode::FAILURE;
    }


    //  The operator
    // -------------
    values = Parse("operator", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique operator for the new data : " 
              << dataName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string op =  *(values.begin()); // The OPERATOR

    bool ok = false;
    unsigned int dim = 0;
    for(std::vector<std::pair<std::string,unsigned int> >::iterator iop = m_operators.begin();iop!=m_operators.end();++iop){
      if( op == (*iop).first){ok=true;dim=(*iop).second;break;}
    }
    if(!ok){
      fatal() << "requested operator "<< op <<" is not allowed " << endmsg;
      info() << "allowed operators are : " << endmsg;
      for(std::vector<std::pair<std::string, unsigned int> >::iterator  it = m_operators.begin();it!=m_operators.end();it++){
        info() << "--> " << (*it).first << endmsg;
        return StatusCode::FAILURE;
      } 
    }


    // the operands
    //--------------
    std::vector<std::string> operands = Parse("data", *iconfig);
    if(dim != operands.size() ){
      fatal() << "Number of operands " << values.size() 
              << "does not match the dimension of the operator " << op << "(dim = " << dim << ")"<<endmsg;
      return StatusCode::FAILURE;
    }

    for(std::vector<std::string>::iterator iop = operands.begin() ; iop != operands.end() ; iop++){
      LHCb::L0DUElementaryData::Map::iterator icheck = m_dataMap.find( *iop );
      if( icheck == m_dataMap.end() ){
        error() << "new compound Data is based on an unknown Data '" << *iop  
                << "' ... check  your settings " << endmsg;
        return StatusCode::FAILURE;
      }
    }

    
    // create Data
    LHCb::L0DUElementaryData* data = 
      new LHCb::L0DUElementaryData(id, LHCb::L0DUElementaryData::Compound, dataName, op , operands ) ;
    m_dataMap[dataName]=data;
    id++;
    
    debug() << "Created Data : " << data->description() << endmsg;

  }
  return StatusCode::SUCCESS;
}





//===============================================================
StatusCode L0DUConfigProvider::createConditions(){

  StatusCode sc = createData();
  if(sc.isFailure())return sc;

  
  int id = m_conditionsMap.size();
  for(ConfigIterator iconfig = m_conditions.begin(); iconfig != m_conditions.end() ; ++iconfig){
    std::vector<std::string> values;
  
    // check all tags exist
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_condFlags.begin();iflag!=m_condFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new CONDITION defined via options (num =  " 
                << iconfig-m_conditions.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new CONDITION description are : " << m_condFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }
    
    // The Condition name 
    //--------------------
    values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The CONDITION defined via option (num = " << iconfig-m_conditions.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXX.Conditions +={ {''name=[ConditionName]'', "
             <<" ''data=[dataName]'', ''comparator=[>/</=/!=]'', ''threshold=[value]''} };" << endmsg;

      return StatusCode::FAILURE;  
    }

    std::string conditionName = *(values.begin()); // The NAME 

  // Check the condition already exist or not (if yes overwrite it)
    LHCb::L0DUElementaryCondition::Map::iterator ic = m_conditionsMap.find( conditionName );
    if( ic != m_conditionsMap.end() ){
      warning() << "One L0DUElementaryCondition with name : '" << conditionName
                <<"' already exists  - Please check your settings" << endmsg;
       return StatusCode::FAILURE;
    }


    // The Data
    //------------------
    values = Parse("data", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique dataName for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string data =  *(values.begin()); // The dataName


    // Special case : create RAM(BCID) data on-the-fly
    if( data.rfind("RAM") != std::string::npos && data.rfind("(BCID)") != std::string::npos ){
      int idData = m_dataMap.size();
      LHCb::L0DUElementaryData* ramData = 
        new LHCb::L0DUElementaryData(idData, LHCb::L0DUElementaryData::Constant, data, "Id" , data ) ;
      m_dataMap[data]=ramData;
      // TEMP
      int ind = data.rfind("(BCID)");
      std::string vsn = data.substr(0,ind);
      debug() <<"RAM(BCID) L0DU DATA for RAM vsn = " << vsn << " HAS BEEN DEFINED" <<endmsg;
      //check the RAM version exists
      const std::vector<int> ram = m_condDB->RAMBCID( vsn );
      if( ram.size() == 0){
        fatal() << "RAM(BCID) versions '" << vsn << "' is not registered" << endmsg;
        return StatusCode::FAILURE;
      }
    }
    
    // Check data consistency
    LHCb::L0DUElementaryData::Map::iterator idata = m_dataMap.find( data );
    if( m_dataMap.end() == idata ){
      fatal() << " Can not set-up the '" << conditionName
              << "' L0DU Elementary Condition, "
              << " because the required L0DU Data '" << data 
              << "' is not defined." << endmsg;
      info() << " The predefined L0DU Data are : " << endmsg;
      for (LHCb::L0DUElementaryData::Map::iterator idata = m_dataMap.begin() ; 
           idata != m_dataMap.end() ;idata++){
        info() <<  " -->  "<< (*idata).second->name()  <<  endmsg; 
      }
      return StatusCode::FAILURE;
    }

    // the Comparator
    //---------------
    values = Parse("comparator", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique comparator for the CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    
    std::string comp =  *(values.begin()); // The COMPARATOR

    bool ok = false;
    for(std::vector<std::string>::iterator icomp = m_comparators.begin();icomp!=m_comparators.end();++icomp){
      if( comp == *icomp){ok=true;break;}
    }
    if(!ok){
      fatal() << "requested comparator "<< comp <<" is not allowed " << endmsg;
      info() << "Allowes comparators are : " << m_comparators << endmsg;
      return StatusCode::FAILURE;
    }
   
    // the Threshold
    //---------------
    values = Parse("threshold", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique threshold for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string cut =  *(values.begin()); // The THRESHOLD
    std::stringstream str("");
    str << cut;
    unsigned long threshold;
    str >> threshold ;


    // the index (facultatif)
    //---------- 
    int index = id;
    values = Parse("index", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUElementaryCondition::Map::iterator ii = m_conditionsMap.begin(); ii!=m_conditionsMap.end();ii++){
      if(index == ((*ii).second)->index() ){
        error() << "The bit index " << index << " is already assigned to the Condition " << ((*ii).second)->name() << endmsg;
        return StatusCode::FAILURE;
      }
    }     

    

    // create condition (name,data,comparator,threshold)
    LHCb::L0DUElementaryCondition* condition = 
      new LHCb::L0DUElementaryCondition(index , conditionName, (*idata).second , comp , threshold);
    m_conditionsMap[conditionName]=condition;
    id++;

    debug() << "Created Condition : " << condition->description() << endmsg;

  }
 
  return StatusCode::SUCCESS;
  
}


//===============================================================
StatusCode L0DUConfigProvider::createChannels(){

  StatusCode sc = createConditions();
  if(sc.isFailure())return sc;
  
  int id = m_channelsMap.size();
  for(ConfigIterator iconfig = m_channels.begin(); iconfig != m_channels.end() ; ++iconfig){
    std::vector<std::string> values;  

    // check all tags exist
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_chanFlags.begin();iflag!=m_chanFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new CHANNEL defined via options (num =  " 
                << iconfig-m_channels.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new CHANNEL description are : " << m_chanFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }    

    // The Channel name 
    //------------------
    values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The CHANNEL defined via option (num = " << iconfig-m_channels.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXX.Channels +={ {''name=[ChannelName]'', "
             <<" ''rate=[value]'', ''condition=[conditionName1],[conditionName2], ...'', ''disable=[FALSE]'' } };" << endmsg;

      return StatusCode::FAILURE;  
    }

    std::string channelName = *(values.begin()); // The NAME

    // Check if the channel already exists 
    LHCb::L0DUChannel::Map::iterator ic = m_channelsMap.find(channelName);
    if( ic != m_channelsMap.end() ){
      warning() << "One L0DUChannel with name = : '" << channelName <<"' already exists " 
                << " - Please check your settings" << endmsg;
      return StatusCode::FAILURE;
    }


    // The rate
    //----------
    values = Parse("rate", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique rate for the new CHANNEL : " 
              << channelName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string srate =  *(values.begin()); // The rate
    std::stringstream str("");
    str << srate;
    double rate;
    str>> rate;

    if(rate<0. || rate > 100.){
      error() << "The channel accept rate " << rate << " (%) rate should be between 0 and 100" << endmsg;
      return StatusCode::FAILURE;
    }

    rate *= ((double) LHCb::L0DUCounter::Scale) / 100.;
    unsigned int irate = (unsigned int) rate;


    // decision mask (facultatif : default == 0x1 == Physics)
    //-----------------------
    std::vector<std::string> enables  = Parse("enable"  , *iconfig);
    std::vector<std::string> disables = Parse("disable" , *iconfig);
    std::vector<std::string> masks    = Parse("mask"    , *iconfig);

    int type = LHCb::L0DUDecision::Physics; // default
    if( enables.size()+disables.size() + masks.size() > 1 ){
      error() << "Is the channel " << channelName << " enabled or disabled ?  Please check your settings." << endmsg;
      return StatusCode::FAILURE;
    }else if( enables.size() == 1){
      Warning("L0DUChannel flag 'Enable' is deprecated - please move to 'Mask' instead",StatusCode::SUCCESS).ignore();
      std::string item(*(enables.begin()));
      std::string uItem(item); 
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ; 
      if( uItem == "FALSE" ){ 
        type = LHCb::L0DUDecision::Disable; 
      }else{ 
        error() << "Decision type for channel '" << channelName << "' is badly defined -  Please check your setting" << endmsg;
        return StatusCode::FAILURE;         
      }          
    }else if( disables.size() == 1){
      Warning("L0DUChannel flag 'Disable' is deprecated - please move to 'Mask' instead",StatusCode::SUCCESS).ignore();
      std::string item(*(disables.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( uItem == "TRUE" ){
        type = LHCb::L0DUDecision::Disable;
      }else{
        error() << "Decision type for channel '" << channelName << "' is badly defined -  Please check your setting" << endmsg;
        return StatusCode::FAILURE;        
      }
    }else if( masks.size() == 1){
      std::string item(*(masks.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( uItem == "1" || uItem == "0X1" || uItem == "001" || uItem == "Physics" ){
        type = LHCb::L0DUDecision::Physics;
      }else if( uItem == "2" || uItem == "0X2" || uItem == "010" || uItem == "Beam1" ){
        type = LHCb::L0DUDecision::Beam1;
      }else if( uItem == "4" || uItem == "0X4" || uItem == "100" || uItem == "Beam2" ){
        type = LHCb::L0DUDecision::Beam2;
      } else if( uItem == "011" || uItem == "3" || uItem == "0x3" ){
        type = 3;
      } else if( uItem == "101" || uItem == "5" || uItem == "0x5" ){
        type = 5;
      } else if( uItem == "110" || uItem == "6" || uItem == "0x6" ){
        type = 6;
      } else if( uItem == "111" || uItem == "7" || uItem == "0x7" ){
        type = 7;
      } else if( uItem == "000" || uItem == "0" || uItem == "0x0" ){
        type = 0;
      }else {
        error() << "Decision type for channel '" << channelName << "' is badly defined -  Please check your setting" << endmsg;
        return StatusCode::FAILURE;        
      }
    }else{
      // no indication => enable the channel for Physics (default)
      type = LHCb::L0DUDecision::Physics; // default
    }

    // the index (facultatif) 
    // ---------
    int index = id;
    values = Parse("index", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new CHANNEL : " 
              << channelName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUChannel::Map::iterator ii = m_channelsMap.begin(); ii!=m_channelsMap.end();ii++){
      if(index == ((*ii).second)->index() ){
        error() << "The bit index " << index << " is already assigned to the Channel " << ((*ii).second)->name() << endmsg;
        return StatusCode::FAILURE;
      }
    }     


    // create channel
    LHCb::L0DUChannel* channel = new LHCb::L0DUChannel(index,  channelName , irate , type ) ;

    // The conditions
    // --------------
    values = Parse("condition", *iconfig);    

    if(values.size() == 0 ){
      error() << "The channel " << channelName << " has no ElementaryCondition" << endmsg;
      return StatusCode::FAILURE;
    }


    // add Elementary Condition(s)
    for(std::vector<std::string>::iterator icond = values.begin() ;icond != values.end() ; icond++){
      // Find required Elementary Condition
      LHCb::L0DUElementaryCondition::Map::iterator ic = m_conditionsMap.find( *icond );
      LHCb::L0DUChannel::Map::iterator icc = m_channelsMap.find( *icond );
      if( m_conditionsMap.end() != ic && m_channelsMap.end() != icc){
        error() << "An ElementaryCondition and a Channel have the same name - please check your setting " << endmsg;
        return StatusCode::FAILURE;
      }
      if( m_conditionsMap.end() == ic ){
        // check if the name  is a previously defined channel
        if( m_channelsMap.end() != icc && *icond != channelName ){
          // YES it is - add all the conditions from this channel
          LHCb::L0DUElementaryCondition::Map condFromChanMap = ((*icc).second)->elementaryConditions();
          for(LHCb::L0DUElementaryCondition::Map::iterator iccc = condFromChanMap.begin();
              iccc!=condFromChanMap.end();iccc++){
            channel->addElementaryCondition ( (*iccc).second ) ;
          }          
        } else{        
          fatal() << " Can not set-up the '" <<  channelName
                  << "' L0DU Channel "
                  << " because the required '" << *icond 
                  << "' is neither a  defined ElementaryCondition nor a defined Channel." << endmsg;
          return StatusCode::FAILURE;
        }
      } else {
        channel->addElementaryCondition ( (*ic).second ) ;
      }
    
    }
    
    debug() << "Created Channel : " << channel->description() << endmsg;
    
    
    m_channelsMap[channelName]=channel;
    id++;
  }
  return StatusCode::SUCCESS;
};



//===============================================================
StatusCode L0DUConfigProvider::createTriggers(){

  // crate channels -> conditions (-> compound data)
  StatusCode sc = createChannels();


  if(m_channels.size()   == 0  || m_conditions.size() == 0)return StatusCode::SUCCESS;

  // pre-defined triggers
  predefinedTriggers();
  
  // Additional user-defined triggers
  if(sc.isFailure())return sc;
  int id = m_triggersMap.size();  
  for(ConfigIterator iconfig = m_triggers.begin(); iconfig != m_triggers.end() ; ++iconfig){
    
    // check all tags exist
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_trigFlags.begin();iflag!=m_trigFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new TRIGGER set defined via options (num =  " 
                << iconfig-m_triggers.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new TRIGGER description are : " << m_trigFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }



    // The Trigger name 
    //------------------
    std::vector<std::string> values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The SubTrigger defined via option (num = " << iconfig-m_channels.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXX.Triggers +={ {''name=[TriggerName]'', "
             <<" ''channels=[channelName1],[channelName2], ...'' } };" << endmsg;

      return StatusCode::FAILURE;  
    }

    
    std::string triggerName = *(values.begin()); // The NAME


    // Check if the triggers set already exists 
    LHCb::L0DUTrigger::Map::iterator ic = m_triggersMap.find(triggerName);
    if( ic != m_triggersMap.end() ){
      warning() << "A L0DU SubTrigger with name  '" << triggerName <<"' already exists " 
                << " (NB : 'L0Ecal', 'L0Hcal', 'L0Muon' and 'Other' SubTrigger names are predefined)" 
                << " - Please check your settings" << endmsg;
      return StatusCode::FAILURE;
    }


    // the index (facultatif) 
    // ---------
    int index = id;
    values = Parse("index", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new SubTrigger: " 
              << triggerName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUTrigger::Map::iterator ii = m_triggersMap.begin(); ii!=m_triggersMap.end();ii++){
      if(index == ((*ii).second)->index() ){
        error() << "The bit index " << index << " is already assigned to the SubTrigger " << ((*ii).second)->name() << endmsg;
        return StatusCode::FAILURE;
      }
    }


    // The decision type (facultatif)
    // -----------------
    std::vector<std::string> types    = Parse("type"    , *iconfig);
    int mask = LHCb::L0DUDecision::Physics;    
    if(types.size() > 0){
      std::string item(*(types.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( item == "PHYSICS" )mask = LHCb::L0DUDecision::Physics;
      else if( item == "BEAM1" )mask = LHCb::L0DUDecision::Beam1;
      else if( item == "BEAM2" )mask = LHCb::L0DUDecision::Beam2;
      else{    
        error() << "Trigger type '" << item << "' is not valid (must be PHYSICS, BEAM1 or BEAM2)" << endmsg;
        return StatusCode::FAILURE;  
      }
    }

    // create trigger
    LHCb::L0DUTrigger* trigger = new LHCb::L0DUTrigger(index,  triggerName , mask) ;


     // The channels 
    // --------------
    std::vector<std::string> channels = Parse("channel", *iconfig);

    if(channels.size() == 0 ){
      error() << "The SubTrigger " << triggerName << " has no Channel" << endmsg;
      return StatusCode::FAILURE;
    }


    



    // check all requested channels exists
    for(std::vector<std::string>::iterator ichan = channels.begin() ;ichan != channels.end() ; ichan++){
      LHCb::L0DUChannel::Map::iterator ic  = m_channelsMap.find( *ichan );
      LHCb::L0DUTrigger::Map::iterator icc = m_triggersMap.find( *ichan );
      if( m_triggersMap.end() != icc && m_channelsMap.end() != ic){
        error() << "A Channel  and a SubTrigger have the same name - please check your setting " << endmsg;
        return StatusCode::FAILURE;
      }
      if( m_channelsMap.end() == ic ){
        if( m_triggersMap.end() != icc && *ichan != triggerName ){
          LHCb::L0DUChannel::Map chanFromTrigMap = ((*icc).second)->channels();
          for(LHCb::L0DUChannel::Map::iterator iccc = chanFromTrigMap.begin(); iccc!=chanFromTrigMap.end();iccc++){
            trigger->addChannel( (*iccc).second );
          }
        } else {
          fatal() << " Can not set-up the '" <<  triggerName
                  << "' L0DU SubTrigger "
                  << " because the required '" << *ichan 
                  << "' is neither a  defined Channel nor a defined SubTrigger." << endmsg;
          return StatusCode::FAILURE;
        }
      } else {
        trigger->addChannel ( (*ic).second );
      }
    }

    // check all channels->decisionType match the trigger->decisionType 
    const LHCb::L0DUChannel::Map& chans = trigger->channels();
    for( LHCb::L0DUChannel::Map::const_iterator ic = chans.begin() ; ic != chans.end() ; ++ic){
      LHCb::L0DUChannel* chan = (*ic).second;
      if( ( chan->decisionType() & trigger->decisionType() ) == 0 ){
        warning() << "The channel '" << LHCb::L0DUDecision::Name[chan->decisionType() ] << "|" << chan->name() 
                  << "' associated to  subTrigger '" 
                  << LHCb::L0DUDecision::Name[trigger->decisionType()] << "|" << trigger->name()
                  << "' will be ignored in the subtrigger decision (decisionType mismatch)"
                  << endmsg;        
      }
    }
    debug() << "Created Trigger  : " << trigger->description() << endmsg; 
    m_triggersMap[triggerName] = trigger;

    id++;
  }   // end loop over subtrigger 

  return StatusCode::SUCCESS;
}


//===============================================================
void L0DUConfigProvider::predefinedTriggers(){

  // create predefined triggers (decisionType = physics)
  for(std::map<std::string,int>::iterator imap = m_tIndices.begin() ; m_tIndices.end() != imap ; imap++){
    LHCb::L0DUTrigger* trigger = new LHCb::L0DUTrigger((*imap).second,  (*imap).first ) ;
    m_triggersMap[ (*imap).first ] = trigger;
  }    

  // Associate one channel to one (or several) trigger(s) according to elementary data (physics decisionType only)
  for(  LHCb::L0DUChannel::Map::iterator ic  = m_channelsMap.begin() ; ic != m_channelsMap.end() ; ic++){
    LHCb::L0DUChannel* channel = (*ic).second;
    if( (channel->decisionType() & LHCb::L0DUDecision::Physics) ==0 )continue;
    // loop over conditions
    std::vector<std::string> dataList;
    for( LHCb::L0DUElementaryCondition::Map::const_iterator ie = channel->elementaryConditions().begin(); 
         ie != channel->elementaryConditions().end() ; ie++){
      const LHCb::L0DUElementaryCondition* condition = (*ie).second;
      const LHCb::L0DUElementaryData* data = condition->data();
      if( !getDataList( data->name() , dataList ) )
        Warning("Cannot associate the data name '" + data->name() +"' to any (list of) L0DUElementaryData").ignore();
    }
    std::vector<std::string> name = triggerNameFromData( dataList );
    for( std::vector<std::string>::iterator it = name.begin() ; it != name.end() ; it++){
      LHCb::L0DUTrigger::Map::iterator imap = m_triggersMap.find( *it );
      if( imap == m_triggersMap.end()){
        error() << " Unknow trigger name '" << *it << "'" << endmsg;
        continue;
      }
      ((*imap).second)->addChannel( channel );
    }
  } 
} 

bool L0DUConfigProvider::getDataList(const std::string dataName, std::vector<std::string>& dataList){
  LHCb::L0DUElementaryData::Map::iterator it = m_dataMap.find( dataName );
  if( it == m_dataMap.end() )return false;
  LHCb::L0DUElementaryData* data = (*it).second;
  bool ok = true;
  if( LHCb::L0DUElementaryData::Predefined == data->type() ){
    dataList.push_back( data->name() );
    return ok;
  }else if(  LHCb::L0DUElementaryData::Compound == data->type() ){
    for(std::vector<std::string>::const_iterator op = data->operandsName().begin() ; op != data->operandsName().end() ; op++ ){
      ok = ok && getDataList( *op , dataList );
    }    
  }
  return ok;
}

std::vector<std::string> L0DUConfigProvider::triggerNameFromData( std::vector<std::string> dataList ){

  std::vector<std::string> name;

  // The actual trigger->data association definition

  // 1st pass for the main trigger : L0Muon, L0Ecal, L0Hcal
  bool hasTrigger = false;
  for(std::vector<std::string>::iterator id = dataList.begin() ; id != dataList.end() ; id++){
    std::string dataName = (*id).substr(0,2);
    if( dataName == "Mu" || dataName == "Di" ){ 
      name.push_back( "L0Muon" ) ;
      hasTrigger = true;
    } else if( dataName == "El" || dataName == "Ph" || dataName == "Lo" || dataName == "Gl" ){
      name.push_back( "L0Ecal" );
      hasTrigger = true;
    } else if( dataName == "Ha" ){
      name.push_back( "L0Hcal" );
      hasTrigger = true;
    }
  }
  if( !hasTrigger)name.push_back( "Other" );
  return name;
}

