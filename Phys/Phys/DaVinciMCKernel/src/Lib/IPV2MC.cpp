// $Id$
// ============================================================================
// $Revision$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// Kernel ?
// ============================================================================
#include "Kernel/IPV2MC.h"
// ============================================================================
/** @file
 *
 * Implementation file for class IPV2MC
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var IID_IPV2MC
   *  unique static identifier of IPV2MC interface 
   *  @see IPV2MC
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-18
   */
  // ==========================================================================
  const InterfaceID IID_IPV2MC ( "IPV2MC" , 1 , 0 ) ;
  // ==========================================================================
}
// ============================================================================
/// Return the unique interface identifier
// ============================================================================
const InterfaceID& IPV2MC::interfaceID() { return IID_IPV2MC; } 
// ============================================================================
/// destructor 
// ============================================================================
IPV2MC::~IPV2MC() {}
// ============================================================================
// The END
// ============================================================================

