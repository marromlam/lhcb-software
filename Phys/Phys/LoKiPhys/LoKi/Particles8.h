// $Id: Particles8.h,v 1.1 2006-02-22 20:53:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES8_H 
#define LOKI_PARTICLES8_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-22 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    class HasProtos
      : public LoKi::Predicate<const LHCb::Particle*>
      , public LoKi::UniqueKeeper<LHCb::ProtoParticle>
    {
    public:
      /// constructor from one protoparticle 
      HasProtos ( const LHCb::ProtoParticle* proto ) ;
      /// constructor from vector of protoparticles 
      HasProtos ( const std::vector<LHCb::ProtoParticle*>& ps ) ;
      /// constructor from vector of protoparticles 
      HasProtos ( const std::vector<const LHCb::ProtoParticle*>& ps ) ;
      /// constructor from container of protoparticles 
      HasProtos ( const LoKi::Keeper<LHCb::ProtoParticle>& ps ) ;
      /// constructor from container of protoparticles 
      HasProtos ( const LoKi::UniqueKeeper<LHCb::ProtoParticle>& ps ) ;
      /** templated constructor from sequence of ptoroparticles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class PROTO>
      HasProtos 
      ( PROTO first , 
        PROTO last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( first , last ) 
      {} ;
      /// copy constructor 
      HasProtos ( const HasProtos& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasProtos() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasProtos* clone() const { return new HasProtos(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& filStream( std::ostream& s ) const ;
    } ;
    
    class HasProtosInTree
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor from one protoparticle 
      HasProtosInTree 
      ( const LHCb::ProtoParticle* proto ) ;
      /// constructor from vector of protoparticles 
      HasProtosInTree
      ( const std::vector<LHCb::ProtoParticle*>& ps ) ;
      /// constructor from vector of protoparticles 
      HasProtosInTree
      ( const std::vector<const LHCb::ProtoParticle*>& ps ) ;
      /// constructor from container of protoparticles 
      HasProtosInTree
      ( const LoKi::Keeper<LHCb::ProtoParticle>& ps ) ;
      /// constructor from container of protoparticles 
      HasProtosInTree
      ( const LoKi::UniqueKeeper<LHCb::ProtoParticle>& ps ) ;
      /** templated constructor from sequence of ptoroparticles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class PROTO>
      HasProtosInTree 
      ( PROTO first , 
        PROTO last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , m_cut ( first , last ) 
      {} ;
      /// copy constructor 
      HasProtosInTree 
      ( const HasProtosInTree& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasProtosInTree() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasProtosInTree* clone() const 
      { return new HasProtosInTree(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private: 
      LoKi::Particles::HasProtos m_cut ;
    } ;
    
  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES8_H
// ============================================================================
