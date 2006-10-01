// $Id: IGenCutTool.h,v 1.5 2006-10-01 22:43:38 robbep Exp $
#ifndef GENERATORS_IGENCUTTOOL_H 
#define GENERATORS_IGENCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/GenCollision.h"

// Forward declarations
namespace HepMC {
  class GenParticle ; 
  class GenEvent ;
}  

namespace LHCb {
  class GenCollision ;
}

class IDecayTool ;

/** @class IGenCutTool IGenCutTool.h "Generators/IGenCutTool.h"
 *  
 *  Abstract interface to generator level cut. This type of cut is applied
 *  to the interaction containing the signal particle. The interaction, at
 *  this point contains undecayed particles (except excited heavy particles).
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IGenCutTool( "IGenCutTool" , 3 , 0 ) ;

class IGenCutTool : virtual public IAlgTool {
public:
  /// Vector of particles
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;

  static const InterfaceID& interfaceID() { return IID_IGenCutTool ; }

  /** Applies the cut on the signal interaction.
   *  @param[in,out] theParticleVector  List of signal particles. The 
   *                                    generator level cut is applied to
   *                                    all these particles and particles
   *                                    which do not pass the cut are removed
   *                                    from theParticleVector.
   *  @param[in]     theGenEvent        Generated interaction. The generator
   *                                    level cut can use the particles in 
   *                                    this event to take the decision.
   *  @param[in]     theCollision       Hard process information of the
   *                                    interaction which can be used by
   *                                    the cut to take the decision.
   *  @param[in]     theDecayTool       tool to generate the signal decay
   *                                    before applying the cut.
   *  @param[in]     cpMixture          indicate the generation of a CP mixture
   *  @param[in]     theSignalAtRest    signal at reset (for forced 
   *                                    fragmentation).
   *  @return        true  if the event passes the generator level cut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theGenEvent ,
                         const LHCb::GenCollision * theCollision ,
                         IDecayTool * theDecayTool , bool cpMixture ,
                         const HepMC::GenParticle * theSignalAtRest ) 
    const = 0 ;
};
#endif // GENERATORS_ICUTTOOL_H
