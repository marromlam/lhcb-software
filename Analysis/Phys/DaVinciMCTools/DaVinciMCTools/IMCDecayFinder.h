// $Id: IMCDecayFinder.h,v 1.3 2002-11-06 08:35:14 odie Exp $
#ifndef TOOLS_IMCDECAYFINDER_H 
#define TOOLS_IMCDECAYFINDER_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/MCParticle.h"
//class MCParticle;

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IMCDecayFinder("IMCDecayFinder", 1, 0);

/** @class IMCDecayFinder IMCDecayFinder.h DaVinciTools/IMCDecayFinder.h
 *
 *  Please see the file MCDecayFinder.h
 *  in the src directory of package Phys/DaVinciTools.
 *
 *  @author Olivier Dormond
 *  @date   12/03/2002
 */
class IMCDecayFinder : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMCDecayFinder; }

  /// Get/Set the decay string to find
  virtual std::string decay( void ) = 0;
  virtual StatusCode setDecay( std::string decay ) = 0;

  /// Does the described decay exists in the event?
  virtual bool hasDecay( const std::vector<MCParticle*> &event ) = 0;
  virtual bool findDecay( const std::vector<MCParticle*> &event,
                          const MCParticle *&previous_result ) = 0;

  virtual bool hasDecay( const MCParticles &event ) = 0;
  virtual bool findDecay( const MCParticles &event,
                          const MCParticle *&previous_result ) = 0;
};
#endif // TOOLS_IMCDECAYFINDER_H
