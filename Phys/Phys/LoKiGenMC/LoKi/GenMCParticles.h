// $Id: GenMCParticles.h,v 1.5 2007-04-16 16:16:45 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2007/02/06 12:44:35  ranjard
// v2r0 - add some include statments to please WIN32
//
// Revision 1.3  2007/01/23 13:29:18  ibelyaev
//  fix for Ex/LoKiExample
//
// Revision 1.2  2006/02/18 18:15:44  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/01/26 17:35:17  ibelyaev
// New import: Generator/MC related part of LoKi project 
// 
// ============================================================================
#ifndef LOKI_GENMCPARTICLES_H 
#define LOKI_GENMCPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Kernel/HepMC2MC.h"
// ============================================================================
// LoKi
// ============================================================================
#include "Relations/IRelation.h"
#include "LoKi/Interface.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/MCTypes.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenExtract.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  namespace GenParticles 
  {   
    /** @class IsAMotherForMC
     *  Trivial predicate which evaluates to 'true'
     *  for (HepMC)particles which are "origins" for 
     *  given certain MCParticles 
     * 
     *  @see LHCb::MCParticle
     *  @see HepMC::GenParticle
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class IsAMotherForMC 
      : public LoKi::Predicate<const HepMC::GenParticle*> 
    {
    public:
      /** constructor from the particle and relation table 
       *  @see IHepMC2MC
       *  @param mc   pointer to MC particle 
       *  @param table relation table  
       */
      IsAMotherForMC 
      ( const LHCb::MCParticle*  mc   , 
        const LHCb::HepMC2MC*    table ) ;
      /** constructor from the particles and relation table 
       *  @see IHepMC2MC
       *  @param mc   range of MC particles 
       *  @param table relation table  
       */
      IsAMotherForMC 
      ( const LHCb::MCParticle::ConstVector&  mc    , 
        const LHCb::HepMC2MC*          table ) ;
      /** constructor from the particles and relation table 
       *  @see IHepMC2MC
       *  @param mc   range of MC particles 
       *  @param table relation table  
       */
      IsAMotherForMC 
      ( const LoKi::MCTypes::MCRange&  mc    , 
        const LHCb::HepMC2MC*          table ) ;
      /** templated constructor from the particles relation table 
       *  @see IHepMC2MC
       *  @param first "begin"-iterator for sequence of MC particles 
       *  @param end   "end"-iterator for sequence of MC particles 
       *  @param table relation table  
       */
      template <class MCPARTICLE>
      IsAMotherForMC 
      ( MCPARTICLE            first ,
        MCPARTICLE            last  , 
        const LHCb::HepMC2MC* table ) 
        : LoKi::Predicate<const HepMC::GenParticle*> ()
        , m_mcps  () 
        , m_table ( table )
      { 
        add ( first , last ) ;
      }
      /** copy constructor 
       *  @param right object to be copied 
       */
      IsAMotherForMC 
      ( const IsAMotherForMC& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~IsAMotherForMC() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsAMotherForMC* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    public:
      IsAMotherForMC& add    ( const LHCb::MCParticle*              mc ) ;
      IsAMotherForMC& add    ( const LHCb::MCParticle::ConstVector& mc ) ;
      IsAMotherForMC& add    ( const LoKi::Types::MCRange&          mc ) ;
      template <class OBJECT>
      IsAMotherForMC& add    ( OBJECT first , OBJECT last  ) 
      {
        for ( ; first != last ; ++first ) { add ( *first ) ; } 
        return *this ;
      }
    private:
      // default constructor is disabled 
      IsAMotherForMC() ;
      // assignement operator is disabled 
      IsAMotherForMC& operator=
      ( const IsAMotherForMC& ) ;
    private:
      typedef LoKi::UniqueKeeper<LHCb::MCParticle>  MCPs    ;
      MCPs                                          m_mcps  ;
      LoKi::Interface<LHCb::HepMC2MC>               m_table ;
    };
    
  } ;  // end of namespace GenParticles
  
  namespace MCParticles 
  {
    /** @class IsFromHepMC
     *  trivial predicate which 
     *  evaluates to 'true' for MC-particles,
     *  which originates from certain HepMC particle 
     *  @see MCParticle
     *  @see IHepMC2MC
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class IsFromHepMC 
      : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /** constructor from the particle and a table 
       *  @see LoKi::Relations::HepMC2MC
       *  @see LoKi::Relations::MC2HepMC
       *  @see LoKi::Relations::HepMC2MC2D
       *  @param p pointer to the particle
       *  @param table pointer to table 
       */
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::MC2HepMC*     table ) ;
      /// constructor from the particle and a table 
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::HepMC2MC*     table ) ;
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::HepMC2MC2D*   table ) ;
      /// constructor from vertex and a tool
      IsFromHepMC
      ( const HepMC::GenVertex*   v     , 
        const LHCb::MC2HepMC*     table ) ;
      /// constructor from vector of particles 
      IsFromHepMC 
      ( const LoKi::GenTypes::GenContainer& p , 
        const LHCb::MC2HepMC* table ) ;
      /// constructor from range of particles 
      IsFromHepMC 
      ( const LoKi::Types::GRange& p , 
        const LHCb::MC2HepMC* table ) ;
      /// constructor from range of particles 
      IsFromHepMC 
      ( const LoKi::Types::GRange& p , 
        const LHCb::HepMC2MC* table ) ;
      /// constructor from range of particles 
      IsFromHepMC 
      ( const LoKi::Types::GRange& p , 
        const LHCb::HepMC2MC2D* table ) ;
      /// constructor from range of objects
      template <class GENOBJECT>
      IsFromHepMC
      ( GENOBJECT             first , 
        GENOBJECT             last  , 
        const LHCb::MC2HepMC* table  ) 
        : LoKi::Predicate<const LHCb::MCParticle*> ()
        , m_gps   () 
        , m_table ( table  )
      { 
        add ( first , last ) ; 
      } ;
      /// constructor from range of objects
      template <class GENOBJECT>
      IsFromHepMC
      ( const LHCb::MC2HepMC* table , 
        GENOBJECT             first , 
        GENOBJECT             last  ) 
        : LoKi::Predicate<const LHCb::MCParticle*> () 
        , m_gps () 
        , m_table ( table )
      { 
        add ( first , last ) ; 
      } ;
      /// copy constructor
      IsFromHepMC 
      ( const IsFromHepMC& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~IsFromHepMC() ;
      /// MANDATORY: clone method("virtual constructor")
      virtual  IsFromHepMC* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      // add a genparticle 
      IsFromHepMC& add ( const HepMC::GenParticle*  p ) ;
      // add a genparticle 
      IsFromHepMC& add ( const HepMC::GenVertex*    p ) ;      
      // add set of genparticles 
      IsFromHepMC& add ( const LoKi::Types::GRange& r )  ;
      // add a sequence
      template <class OBJECT>
      IsFromHepMC& add ( OBJECT first , OBJECT last  ) 
      { 
        for ( ; first != last ; ++first ) { add (*first ) ; } 
        return *this ;
      }
    public:
      IsFromHepMC& set ( const LHCb::MC2HepMC*   table ) ;
      IsFromHepMC& set ( const LHCb::HepMC2MC*   table ) ;      
      IsFromHepMC& set ( const LHCb::HepMC2MC2D* table ) ;      
    private:
      // default constructor is disabled
      IsFromHepMC();
      // assignement operator is disabled 
      IsFromHepMC& operator=
      ( const IsFromHepMC& ) ;
    private:
      typedef LoKi::UniqueKeeper<HepMC::GenParticle> GPs     ;
      GPs                                            m_gps   ;
      LoKi::Interface<LHCb::MC2HepMC>                m_table ;
    } ;
    
  } ; // end of namespace MCParticles
  
} ; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENMCPARTICLES_H
// ============================================================================
