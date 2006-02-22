// $Id: Particles4.h,v 1.2 2006-02-22 20:53:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES4_H 
#define LOKI_PARTICLES4_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/PrimVertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Keeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/ImpParBase.h"
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
 *  @date 2006-02-19 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    /** @class ImpPar
     *  class for evaluation of impact parameter of 
     *  particle with respect to vertex
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::IP
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpPar : 
      public    LoKi::Function<const LHCb::Particle*> , 
      public    LoKi::Vertices::ImpParBase 
    {
    public:
      /// constructor 
      ImpPar 
      ( const LHCb::Vertex*                     vertex ,   
        const LoKi::Vertices::ImpactParamTool&  tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Point3D&                     point  ,   
        const LoKi::Vertices::ImpactParamTool&   tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   ,
        const LHCb::Vertex*                      vertex ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   , 
        const LoKi::Point3D&                     point  ) ;
      /// constructor 
      ImpPar ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// copy constructor 
      ImpPar ( const LoKi::Particles::ImpPar&    right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpPar(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual ImpPar* clone() const { return new ImpPar( *this ) ; }        
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return ip( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type ip ( argument p ) const ;
    private:
      // default constructor is private 
      ImpPar();
    };
    
    /** @class ImpParChi2
     *  class for evaluation of impact parameter chi2 
     *  of particle with respect to vertex
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::IPCHI2
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpParChi2 : 
      public    LoKi::Function<const LHCb::Particle*> , 
      public    LoKi::Vertices::ImpParBase 
    {
    public:
      /// constructor 
      ImpParChi2 
      ( const LHCb::Vertex*                    vertex ,   
        const LoKi::Vertices::ImpactParamTool& tool   ) ;
      /// constructor 
      ImpParChi2 
      ( const LoKi::Point3D&                   point  ,   
        const LoKi::Vertices::ImpactParamTool& tool   ) ;
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   ,
        const LHCb::Vertex*                    vertex ) ;
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::Point3D&                   point  ) ;
      /// constructor 
      ImpParChi2 ( const LoKi::Vertices::ImpParBase&  tool  ) ;
      /// copy constructor 
      ImpParChi2 ( const LoKi::Particles::ImpParChi2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpParChi2(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ImpParChi2* clone() const {return new ImpParChi2(*this) ;} ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return chi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type chi2 ( argument p ) const ;
    private:
      // default constructor is private 
      ImpParChi2();
    };
    
    /** @class MinImpPar
     *  class for evaluation of minimal valeu of 
     *  impact parameter of 
     *  particle with respect to seevral vertices 
     *
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LoKi::Vertices::ImpPar 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::MINIP
     *  @see LoKi::Cuts::IPMIN
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MinImpPar 
      : public LoKi::Function<const LHCb::Particle*> 
      , public LoKi::Keeper<LHCb::Vertex> 
    {
    public:
      /// constructor from vertices and the tool
      MinImpPar
      ( const LHCb::Vertex::Vector& vertices         , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::Vertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const SmartRefVector<LHCb::Vertex>& vertices , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::PhysTypes::VRange& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::PrimVertex::Vector& vertices         , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::PrimVertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const SmartRefVector<LHCb::PrimVertex>& vertices , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::PrimVertices* vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::Vector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const SmartRefVector<LHCb::Vertex>& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LoKi::PhysTypes::VRange& vertices   ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::PrimVertex::Vector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::PrimVertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool&  tool     ,
        const SmartRefVector<LHCb::PrimVertex>& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool     , 
        const LHCb::PrimVertices*              vertices ) ;
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpPar 
      ( VERTEX                                 first , 
        VERTEX                                 last  , 
        const LoKi::Vertices::ImpactParamTool& tool  ) 
        : LoKi::Function<const LHCb::Particle*> ()
        , LoKi::Keeper<LHCb::Vertex>( first , last )
        , m_fun      ( (const LHCb::Vertex*) 0 , tool )
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param tool  helper tool 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      MinImpPar 
      ( const LoKi::Vertices::ImpactParamTool& tool  ,
        VERTEX                                 first , 
        VERTEX                                 last  ) 
        : LoKi::Function<const LHCb::Particle*> ()
        , LoKi::Keeper<LHCb::Vertex>( first , last )
        , m_fun      ( (const LHCb::Vertex*) 0 , tool )
      {};
      /// copy constructor 
      MinImpPar ( const LoKi::Particles::MinImpPar& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpPar(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpPar* clone() const { return new MinImpPar(*this) ; };
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return mip( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type mip ( argument p ) const ;
    public:
      // default constructor is private 
      MinImpPar();
    private:
      // the actual IP evaluator 
      LoKi::Particles::ImpPar m_fun ; ///< the actual IP evaluator 
    };

    /** @class MinImpParChi2
     *  class for evaluation of minimal value of 
     *  chi2 of impact parameter of 
     *  particle with respect to seevral vertices 
     *
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LoKi::Vertices::ImpParChi2 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::MIPCHI2
     *  @see LoKi::Cuts::CHI2MIP
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MinImpParChi2 
      : public LoKi::Function<const LHCb::Particle*> 
      , public LoKi::Keeper<LHCb::Vertex> 
    {
    public:
      /// constructor from vertices and the tool
      MinImpParChi2
      ( const LHCb::Vertex::Vector& vertices         , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::Vertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const SmartRefVector<LHCb::Vertex>& vertices , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::PhysTypes::VRange& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::PrimVertex::Vector& vertices         , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::PrimVertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const SmartRefVector<LHCb::PrimVertex>& vertices , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::PrimVertices* vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::Vector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const SmartRefVector<LHCb::Vertex>& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LoKi::PhysTypes::VRange& vertices   ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::PrimVertex::Vector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::PrimVertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool&  tool     ,
        const SmartRefVector<LHCb::PrimVertex>& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool     , 
        const LHCb::PrimVertices*              vertices ) ;
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( VERTEX                                 first , 
        VERTEX                                 last  , 
        const LoKi::Vertices::ImpactParamTool& tool  ) 
        : LoKi::Function<const LHCb::Particle*> ()
        , LoKi::Keeper<LHCb::Vertex>( first , last )
        , m_fun      ( (const LHCb::Vertex*) 0 , tool )
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param tool  helper tool 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool  ,
        VERTEX                                 first , 
        VERTEX                                 last  ) 
        : LoKi::Function<const LHCb::Particle*> ()
        , LoKi::Keeper<LHCb::Vertex>( first , last )
        , m_fun      ( (const LHCb::Vertex*) 0 , tool )
      {};
      /// copy constructor 
      MinImpParChi2 ( const LoKi::Particles::MinImpParChi2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpParChi2(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParChi2* clone() const 
      { return new MinImpParChi2(*this) ; };
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return mipchi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type mipchi2 ( argument p ) const ;
    public:
      // default constructor is private 
      MinImpParChi2();
    private:
      // the actual IP evaluator 
      LoKi::Particles::ImpParChi2 m_fun ; ///< the actual IP evaluator 
    };
    
  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES4_H
// ============================================================================
