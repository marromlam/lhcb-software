// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/iNode.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/KinTypes.h"
#include "LoKi/ToCpp.h"
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    std::string toCpp ( const LoKi::LorentzVector&v ) 
    {
      return 
        "LoKi::LorentzVector(" 
        + toCpp ( v.Px() ) + "," 
        + toCpp ( v.Py() ) + "," 
        + toCpp ( v.Pz() ) + "," 
        + toCpp ( v.E () ) + ")" ;  
    }
    // ========================================================================
    std::string toCpp ( const LoKi::ThreeVector&v ) 
    {
      return 
        "LoKi::ThreeVector(" 
        + toCpp ( v.X () ) + "," 
        + toCpp ( v.Y () ) + "," 
        + toCpp ( v.Z () ) + ")";
    }
    // ========================================================================
    std::string toCpp ( const LoKi::Point3D& v ) 
    {
      return 
        "LoKi::Point3D(" 
        + toCpp ( v.X() ) + "," 
        + toCpp ( v.Y() ) + "," 
        + toCpp ( v.Z() ) + ")";
    }
    // ========================================================================
    std::string toCpp ( const LHCb::ParticleID& v ) 
    { return "LHCb::ParticleID(" + toCpp ( v.pid() ) + ")" ; }
    // ========================================================================
    std::string toCpp ( const Decays::iNode& v ) 
    { return "\"" + v.toString() + "\"" ; }
    // ========================================================================
  }    
  // ==========================================================================
}
// ============================================================================
// The END
// ============================================================================


