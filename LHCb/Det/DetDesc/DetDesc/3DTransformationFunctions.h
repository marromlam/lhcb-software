// $Id: 3DTransformationFunctions.h,v 1.4 2008-01-11 14:22:26 jpalac Exp $
#ifndef DETDESC_3DTRANSFORMATIONFUNCTIONS_H 
#define DETDESC_3DTRANSFORMATIONFUNCTIONS_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"
#include "Math/Translation3D.h"
#include <vector>

namespace DetDesc {

  /**
   * Build the local to global transformation matrix given 
   *
   * @author Juan Palacios
   * @date   2007-06-13
   *
   * @param translationParams vector containing the X,Y,Z coordinates of the translation
   *        part of the transformation
   * @param rotationParams vector containing the rotation angles about the X,Y,Z axes. 
   *        The rotation is applied in the following order: Z-Y'-X"
   * @param pivotParams vector containing the X,Y,Z coordinates of a pivot point for the
   *        rotation part of the transformation.
   * @return The full 3D transformation object. 
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  const Gaudi::Transform3D localToGlobalTransformation(const std::vector<double>& translationParams,
                                                       const std::vector<double>& rotationParams,
                                                       const std::vector<double>& pivotParams);


  /**
   * Get the three rotation angles and three translation coordinates from a 3D transformation
   * about a pivot point.
   * 
   * @author Juan Palacios juan.palacios@nikhef.nl
   * 
   * @param CDM 3D transformation object AKA cosine direction matrix
   * @param translationParams vector containing X,Y and Z of translation transformation.
   * @param rotationParams vector containing the rotations about the X,Y,Z axes. 
   *                       The rotations are applied in the following order: Z-Y'-X"
   * @param pivotParams    pivot point about which the 3D transformation is to be decomposed.
   *
   */
  void getZYXTransformParameters(const Gaudi::Transform3D& CDM,
                                 std::vector<double>& translationParams,
                                 std::vector<double>& rotationParams,
                                 const std::vector<double>& pivotParams = std::vector<double>(3,0.));

  
}

#endif // DETDESC_3DTRANSFORMATIONFUNCTIONS_H
