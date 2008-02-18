// $Id: IPatSeedingTool.h,v 1.3 2008-02-18 14:42:45 mschille Exp $
#ifndef TRACKINTERFACES_IPATSEEDINGTOOL_H 
#define TRACKINTERFACES_IPATSEEDINGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Track.h"

static const InterfaceID IID_IPatSeedingTool ( "IPatSeedingTool", 1, 0 );

// forward declaration
namespace LHCb {
  class State;
}

/** @class IPatSeedingTool IPatSeedingTool.h TrackInterfaces/IPatSeedingTool.h
 *  Interface to the forward pattern tool
 *
 *  @author Olivier Callot
 *  @date   2005-10-04
 */
class IPatSeedingTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatSeedingTool; }

  virtual StatusCode performTracking(
		 LHCb::Tracks* output,
		 const LHCb::State* state = 0 ) = 0; 

  virtual StatusCode performTracking(
		 std::vector<LHCb::Track*>& outputTracks,
		 const LHCb::State* state = 0 ) = 0;

  virtual void prepareHits () = 0; 

 
protected:

private:

};
#endif // TRACKINTERFACES_IPATSEEDINGTOOL_H
