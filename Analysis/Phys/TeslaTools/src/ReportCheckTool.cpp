// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "ReportCheckTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReportCheckTool: ReportCheckTool.cpp
// Author: Sean Benson
// 05/06/2008
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ReportCheckTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReportCheckTool::ReportCheckTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTool( type, name , parent )
{
  declareInterface<IReportCheck>(this);
  
  // Associate versions with saved particle properties
  version_map_particle.emplace("1#Particle.measuredMass",1);
  version_map_particle.emplace("9#Particle.massErr",2);
  // Associate versions with saved track properties
  version_map_track.emplace("6#Track.chi2PerDoF",1);
  version_map_track.emplace("8#Track.Likelihood",2);
  // Associate versions with saved vertex properties
  version_map_vertex.emplace("0#Vertex.chi2",2);
  // Associate versions with saved rpid properties
  version_map_rpid.emplace("4#Rich.DLLK",2);
}

//===========================================================================
/// ReportCheckTool::VersionTopLevel finds report version from HltSelReports.
//===========================================================================
int ReportCheckTool::VersionTopLevel(string trigger)
{
  if (msgLevel(MSG::DEBUG)) debug() << "SelReports version check requested" << endmsg;
  
  int vnum = 99;

  const LHCb::HltSelReports* selReports;
  if ( exist<LHCb::HltSelReports>( LHCb::HltSelReportsLocation::Default ) ) {
    selReports = get<LHCb::HltSelReports>( LHCb::HltSelReportsLocation::Default );
  }
  else{
    warning() << "Sel. reports do not exist!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  const LHCb::HltObjectSummary * MotherRep = selReports->selReport(trigger.c_str());
  if(MotherRep){
    
    debug() << "Examining mother report" << endmsg;

    SmartRefVector <LHCb::HltObjectSummary> substructure = MotherRep->substructure();
    const LHCb::HltObjectSummary * Obj = substructure.begin()->target();
    // What keys are present:
    std::vector<string> infoList;
    std::vector<string> vec_keys = Obj->numericalInfoKeys();

    if( Obj->summarizedObjectCLID() == LHCb::Particle::classID() ){
      
      debug() << "Mother report is of a particle" << endmsg;

      for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) {
        string key = *it;
        if( version_map_particle.find( key )->second == 2 ) vnum = 2;
        else if( version_map_particle.find( key )->second == 1 ) vnum = 1;
      }
      if(vnum==99){
        warning() << "Please check how HltSelReports were created" << endmsg;
        return 0;
      }
    }
    else if( Obj->summarizedObjectCLID() == LHCb::Track::classID() ){
      
      debug() << "Mother report is of a track" << endmsg;
      
      for(std::vector<std::string>::iterator it = vec_keys.begin(); it!=vec_keys.end(); it++) {
        string key = *it;
        if( version_map_track.find( key )->second == 2 ) vnum = 2;
        else if( version_map_track.find( key )->second == 1 ) vnum = 1;
      }
      if(vnum==99){
        warning() << "Please check how HltSelReports were created" << endmsg;
        return 0;
      }
    }
  }
  return vnum;
}

