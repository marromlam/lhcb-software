// $Id: RichFunctionalRayleighScatter.h,v 1.2 2003-12-11 16:33:36 cattanem Exp $
#ifndef RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichRayleighScatter.h"

/** @class RichFunctionalRayleighScatter RichFunctionalRayleighScatter.h
 *
 *  Tool to calculate quantities related to Rayleigh scattering using a
 *  analytic functional form
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichFunctionalRayleighScatter : public RichRecToolBase,
                                      virtual public IRichRayleighScatter {

public:

  /// Standard constructor
  RichFunctionalRayleighScatter( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichFunctionalRayleighScatter() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Rayleigh scattering probability for given pathlength (segment) and photyon energy
  double photonScatteredProb( const RichRecSegment * segment,
                              const double energy );

private:  // Private data

  // Rayleigh sattering parameters
  double m_eVToMicron;  ///< To converter between energy in eV to wavelenght in micrometers
  double m_AeroClarity; ///< The areogel clarity

};

#endif // RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H
