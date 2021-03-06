// $Id: Interfaces.cpp,v 1.3 2009-04-17 08:54:24 cattanem Exp $
// ============================================================================
// Include files
// ============================================================================
#include "DetDesc/ISolid.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ILVolume.h"
// ============================================================================

// ============================================================================
/** @file 
 *  (empty) Implementaton of virtual destrctors for abstract classes 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
IDetectorElement:: ~IDetectorElement () {}
// ============================================================================
IGeometryInfo::    ~IGeometryInfo    () {}
// ============================================================================
ISolid::           ~ISolid           () {}
// ============================================================================
ILVolume::         ~ILVolume         () {}
// ============================================================================
IPVolume::         ~IPVolume         () {}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
