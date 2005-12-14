
//----------------------------------------------------------------------------
/** @file DeRichRadiator.h
 *
 *  Header file for detector description class : DeRichRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichRadiator.h,v 1.16 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHRADIATOR_H
#define RICHDET_DERICHRADIATOR_H 1

// Include files
#include "Kernel/Point3DTypes.h"

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"

// GaudiKernel
class IAlgTool;

/** @namespace DeRichRadiatorLocation
 *
 *  Namespace for the location of radiators in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichRadiatorLocation {
  /// Multi solid aerogel location
  static const std::string& Aerogel_old   = "/dd/Structure/LHCb/Rich1/Aerogel";
  static const std::string& Aerogel = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Aerogel";

  /// Rich1 gas (C4F10) location
  static const std::string& C4F10_old     = "/dd/Structure/LHCb/Rich1/C4F10";
  static const std::string& C4F10 = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/C4F10";

  /// Rich2 gas (CF4) location
  static const std::string& CF4_old       = "/dd/Structure/LHCb/Rich2/CF4";
  static const std::string& CF4  = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/CF4";
}

//----------------------------------------------------------------------------
/** @class DeRichRadiator DeRichRadiator.h
 *
 * DeRichRadiator provides information about the Rich radiators.
 * This is the common base class of the Rich Single/Multi Solid Radiator
 * detector classes
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
//----------------------------------------------------------------------------

class DeRichRadiator: public DetectorElement {

public:

  /// Default constructor
  DeRichRadiator()
    : DetectorElement (),
      m_radiatorID    ( Rich::InvalidRadiator ),
      m_rich          ( Rich::InvalidDetector ),
      m_refIndex      ( 0                     ),
      m_rayleigh      ( 0                     ),
      m_name          ( "UnInitialized"       ),
      m_condTool      ( 0                     ) { }

  /// Destructor
  virtual ~DeRichRadiator();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Retrieves the id of this radiator
   * @return The id of this radiator (Aerogel, C4F10, CF4)
   */
  inline Rich::RadiatorType radiatorID() const
  {
    return m_radiatorID;
  }

  /**
   * Retrieves the rich detector of this radiator
   * @return the rich detector of this radiator (Rich1/2)
   */
  inline Rich::DetectorType rich() const
  {
    return m_rich;
  }

  /**
   * Retrieves the refractive index of the radiator
   * @return A pointer to the refractive index of the radiator
   */
  inline const TabulatedProperty* refIndex() const
  {
    return m_refIndex;
  }

  /**
   * Retrieves the Rayleigh properties of the radiator
   * @return A pointer to the Rayleigh tab property
   * @retval NULL No Rayleigh tab property
   */
  inline const TabulatedProperty* rayleigh() const
  {
    return m_rayleigh;
  }

  /**
   * Finds the next intersection point with radiator.
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  virtual StatusCode nextIntersectionPoint( const Gaudi::XYZPoint& pGlobal,
                                            const Gaudi::XYZVector& vGlobal,
                                            Gaudi::XYZPoint& returnPoint ) const = 0;

  /**
   * Finds the entry and exit points of the radiator. For boolean solids
   * this is the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                         const Gaudi::XYZVector& vGlobal,
                                         Gaudi::XYZPoint& entryPoint,
                                         Gaudi::XYZPoint& exitPoint ) const = 0;

  /**
   * Finds the intersection points with radiator. For boolean solids there
   * can be more than two intersection points
   *
   * @return The number of intersection points.
   * @retval Zero if there is no intersction.
   */
  virtual unsigned int intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                           const Gaudi::XYZVector& vGlobal,
                                           std::vector<Gaudi::XYZPoint>& points ) const = 0;


  /** Returns the name of this particular radiator medium
   *  @return radiator name
   */
  inline const std::string & myName() const { return m_name; }

protected:

  Rich::RadiatorType m_radiatorID;  ///< The radiator id (Aerogel, CF4, C4F10)
  Rich::DetectorType m_rich;        ///< The Rich detector of this radiator

  /// pointer to the refractive index of the material
  const TabulatedProperty* m_refIndex;

  /// pointer to the Rayleigh scattering properties
  const TabulatedProperty* m_rayleigh;

private:

  std::string m_name; ///< The name of this radiator

  /// Pointer to RichCondition tool that maintains the
  /// refractive index tabulated properties
  IAlgTool * m_condTool;

};

#endif    // RICHDET_DERICHRADIATOR_H
