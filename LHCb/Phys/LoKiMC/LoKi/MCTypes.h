// $Id: MCTypes.h,v 1.2 2006-02-18 18:10:57 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/26 16:13:39  ibelyaev
// New Packaage: MC-dependent part of LoKi project 
//
// ============================================================================
#ifndef LOKI_MCTYPES_H 
#define LOKI_MCTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Selected.h"
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
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  /** @namespace  MCTypes MCTypes.h LoKi/MCTypes.h
   *  
   *  Helper namespace to collect some useful MC data types 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-05-11
   */
  namespace MCTypes 
  { 
    
    typedef std::vector<LHCb::MCParticle*> MCParticleVector ;
    typedef std::vector<LHCb::MCVertex*>   MCVertexVector   ;
    
    typedef MCParticleVector               MCContainer      ;
    typedef LoKi::Selected_<MCContainer>   MCSelected       ;
    
    typedef MCVertexVector                 MCVContainer     ;
    typedef LoKi::Selected_<MCVContainer>  MCVSelected      ;
    typedef MCSelected::Range              MCRange          ;
    typedef MCVSelected::Range             MCVRange         ;
    
    /// type of 'cuts' for MCParticles       (interface)
    typedef LoKi::Predicate<const LHCb::MCParticle*>              MCCuts  ;
    /// type of 'functions' for <MCParticles  (interface)
    typedef LoKi::Function<const LHCb::MCParticle*>               MCFunc  ;
    /// type of 'cuts' for MCParticles       (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::MCParticle*> MCCut   ;
    /// type of 'functions' for MCParticles  (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::MCParticle*>   MCFun   ;
    
    /// type of 'cuts' for MCVertices      (interface)
    typedef LoKi::Predicate<const LHCb::MCVertex*>                MCVCuts ;
    /// type of 'functions' for MCVertices (interface)
    typedef LoKi::Function<const LHCb::MCVertex*>                 MCVFunc ;
    /// type of 'cuts' for Vertices      (assignable)
    typedef LoKi::PredicateFromPredicate<const LHCb::MCVertex*>   MCVCut  ;
    /// type of 'functions' for Vertices (assignable)
    typedef LoKi::FunctionFromFunction<const LHCb::MCVertex*>     MCVFun  ;
    
  }; // end of namespace MCTypes
  
  namespace Types 
  {
    /// type of 'cuts' for MCParticles       (interface)
    typedef LoKi::MCTypes::MCCuts  MCCuts ;
    /// type of 'functions' for <MCParticles  (interface)
    typedef LoKi::MCTypes::MCFunc  MCFunc ;
    /// type of 'cuts' for MCParticles       (assignable)
    typedef LoKi::MCTypes::MCCut   MCCut  ;
    /// type of 'functions' for MCParticles  (assignable)
    typedef LoKi::MCTypes::MCFun   MCFun  ;
    
    /// type of 'cuts' for MCVertices        (interface)
    typedef LoKi::MCTypes::MCVCuts MCVCuts ;
    /// type of 'functions' for MCVertices   (interface)
    typedef LoKi::MCTypes::MCVFunc MCVFunc ;
    /// type of 'cuts' for Vertices          (assignable)
    typedef LoKi::MCTypes::MCVCut  MCVCut  ;
    /// type of 'functions' for Vertices     (assignable)
    typedef LoKi::MCTypes::MCVFun  MCVFun  ;
    
  };
  
}; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCTYPES_H
// ============================================================================
