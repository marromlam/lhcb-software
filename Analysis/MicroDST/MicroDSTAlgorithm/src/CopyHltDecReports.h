// $Id: CopyHltDecReports.h,v 1.1 2009-04-03 10:36:46 jpalac Exp $
#ifndef COPYHLTDECREPORTS_H 
#define COPYHLTDECREPORTS_H 1

// Include files
// MicroDST
#include "MicroDST/ObjectClonerAlg.h"
#include "MicroDST/BindType2Cloner.h"
// from LHCb
#include "Event/HltDecReports.h"

/** @class CopyHltDecReports CopyHltDecReports.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::HltDecReports from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::HltDecReports is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::HltDecReportsLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::HltDecReports from default location ("DAQ/HltDecReports") 
 *  to "/Event/MyLocation/Hlt/HltDecReports"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"MicroDST::ObjectClonerAlg<LHCb::HltDecReports>/CopyHltDecReports"};
 *  CopyHltDecReports.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-04-03
 */
//=============================================================================
typedef BindType2Cloner<LHCb::HltDecReports> __cloner;
//=============================================================================
template <> struct Location<LHCb::HltDecReports> {
  const static std::string Default;
};  
const std::string Location<LHCb::HltDecReports>::Default = LHCb::HltDecReportsLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::HltDecReports> CopyHltDecReports;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyHltDecReports, CopyHltDecReports )

#endif // COPYHLTDECREPORTS_H
