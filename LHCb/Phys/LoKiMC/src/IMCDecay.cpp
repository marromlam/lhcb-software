// $Id: IMCDecay.cpp,v 1.1 2009-05-23 15:58:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IMCDecay.h"
// ============================================================================
/** @file 
 *  Implementation file for class Decays::IMCDecay
 *  @author Ivan BELYAEV
 *  @date   2009-05-22
 */
// ============================================================================
// virtual & protected destructor 
// ============================================================================
Decays::IMCDecay::~IMCDecay () {}             // virtual & protected destructor 
// ============================================================================
// unique interface ID
// ============================================================================
const InterfaceID& Decays::IMCDecay::interfaceID()   // the unique interface ID
{
  static const InterfaceID s_ID ( "Decays::IMCDecay" , 1 , 0 ) ;
  return s_ID ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
