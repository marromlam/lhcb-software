// $Id: IHltSummaryTool.h,v 1.2 2007-02-14 17:23:51 cattanem Exp $
#ifndef HLTBASE_IHLTSUMMARYTOOL_H 
#define HLTBASE_IHLTSUMMARYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class HltSummary;
  class State;
};

static const InterfaceID IID_IHltSummaryTool ( "IHltSummaryTool", 1, 0 );

/** @class IHltSummaryTool IHltSummaryTool.h tmp/IHltSummaryTool.h
 *
 *  functionality: 
 *      tool to simplify common task asked to the HltSummary  
 *
 *  @author Jose A. Hernando
 *  @date   2007-02-08
 */
class IHltSummaryTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltSummaryTool; }

  virtual StatusCode load() = 0;

  virtual void setSummary(const LHCb::HltSummary& summary) = 0;

  virtual bool isInSelection(const LHCb::Track& track, int selID ) = 0;

};
#endif // HLTBASE_IHLTSUMMARYTOOL_H
