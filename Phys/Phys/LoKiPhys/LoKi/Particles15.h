// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES15_H 
#define LOKI_PARTICLES15_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Interface.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-20 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class Filter
     *  Simple adapter which allos to use the tool IParticleFilter
     *  as LoKi functor
     *  @see IParticleFilter
     *  @see LoKi::Cuts::FILTER
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-20
     */
    class GAUDI_API Filter
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the filter name 
      Filter ( const std::string&      filter ) ;
      /// constructor from the filter 
      Filter ( const IParticleFilter*  filter ) ;
      /// MANDATORY: virtual destructor
      virtual ~Filter() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Filter* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      Filter () ;                            // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<IParticleFilter> m_filter ;            // the tool itself 
      // ======================================================================
    } ;
    // ========================================================================    
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES15_H
// ============================================================================
