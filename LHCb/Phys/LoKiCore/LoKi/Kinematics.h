// $Id: Kinematics.h,v 1.15 2008-07-27 18:19:27 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_KINEMATICS_H 
#define LOKI_KINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/KinTypes.h"
#include "LoKi/Constants.h"
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
  // ===========================================================================
  /** @namespace LoKi::Kinematics 
   *  collection of few trivial functions for kinematical evaluations
   *
   *  @todo uncomment the implementation of 
   *  "transversity angle" and "forward-backward angle"
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-17
   */
  namespace Kinematics 
  {
    // ========================================================================
    /** simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *  (E**2+Px**2+Py**2+Pz**2)
     *  @param vct the vector
     *  @return euclidian norm squared   
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double euclidianNorm2 
    ( const LoKi::LorentzVector& vct ) ;
    // ========================================================================
    /** simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *  sqrt(E**2+Px**2+Py**2+Pz**2)
     *  @param vct the vector
     *  @return euclidian norm    
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double euclidianNorm
    ( const LoKi::LorentzVector& vct ) ;
    // ========================================================================
    /** simple function for evaluation of the square of 
     *  the euclidian distance inbetwee 2 LorentzVectors 
     *  (DeltaE**2+DeltaPx**2+DeltaPy**2+DeltaPz**2)
     *  @param vct1 the first vector
     *  @param vct2 the second vector
     *  @return the square of euclidian distance 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double delta2euclidian 
    ( const LoKi::LorentzVector& vct1 , 
      const LoKi::LorentzVector& vct2 ) ;
    // ========================================================================    
    /** simple function which evaluates the transverse 
     *  momentum with respect a certain 3D-direction
     *  @param mom the momentum
     *  @param dir the direction
     *  @return the transverse moementum with respect to the direction
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double transverseMomentumDir 
    ( const LoKi::LorentzVector& mom , 
      const LoKi::ThreeVector&   dir ) ;
    // ========================================================================    
    /** trivial function to evaluate the mass of 4-vector 
     *  @param mom lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& mom ) { return mom.M() ; }
    // ========================================================================    
    /** trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first lorenz vector 
     *  @param v2  the second lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ) { return mass ( v1 + v2 ) ; }
    // ========================================================================    
    /** trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first  lorenz vector 
     *  @param v2  the second lorenz vector
     *  @param v3  the third  lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ,
      const LoKi::LorentzVector& v3 ) 
    { 
      LoKi::LorentzVector v0( v1 ) ;
      v0 += v2 ;
      v0 += v3 ;      
      return mass ( v0 ) ; 
    } 
    // ========================================================================    
    /** trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first  lorenz vector 
     *  @param v2  the second lorenz vector
     *  @param v3  the third  lorenz vector
     *  @param v4  the fourth  lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ,
      const LoKi::LorentzVector& v3 , 
      const LoKi::LorentzVector& v4 ) 
    {
      LoKi::LorentzVector v0( v1 ) ;
      v0 += v2 ;
      v0 += v3 ;      
      v0 += v4 ;      
      return mass ( v0 ) ; 
    } 
    // ========================================================================
    /** the simple function which adds a 4-momenta of all (MC)Particles
     *  from a sequence [begin,end).
     *  The actual type of elements is irrelevant, 
     *  The function relies only on existence of mehtod "momentum()"
     *  with a return-value convertible to LoKi::LorentzVector
     *
     *  e.g. eny sequence of objects, convertible to 
     *  "const LHCb::Particle*","const LHCb::MCParticle*" or 
     *  "const HepMC::GenParticle*" could be used.
     *
     *  @code 
     *  const LHCb::Vertex*             vertex = ... ;
     *  const LHCb::MCParticles*        mcps   = ... ; 
     *  const LHCb::Particles*          ps     = ... ; 
     *  std::vector<LHCb::MCParticle*>  mcv    = ... ;
     *  Range                           pr     = ... ;
     *  MCRange                         mcr    = ... ;
     *
     *  // get the 4-mometum of all daughter particle at  vertex:
     *  LoKi::LorentzVector sum1 = 
     *     LoKi::Adder::addMomenta( vertex->products().begin() , 
     *                              vertex->products().end()   ) ;
     *  
     *  // get the 4-momentum of all MC particles 
     *  LoKi::LorentzVector sum2 = 
     *     LoKi::Adder::addMomenta( mcps->begin() , mcps->end()   ) ;
     *  
     *  // get the 4-momentum of all particles 
     *  LoKi::LorentzVector sum3 = 
     *     LoKi::Adder::addMomenta( ps->begin() , ps->end()   ) ;
     *  
     *  // get the 4-momentum of all particles 
     *  LoKi::LorentzVector sum4 = 
     *     LoKi::Adder::addMomenta( pv.begin() , pv.end()   ) ;
     *
     *  // get the 4-momentum of all MC particles 
     *  LoKi::LorentzVector sum5 = 
     *     LoKi::Adder::addMomenta( mcv.begin() , mcv.end()   ) ;
     *
     *  // get the 4-momentum of all particles 
     *  LoKi::LorentzVector sum6 = 
     *     LoKi::Adder::addMomenta( pr.begin() , pr.end()   ) ;
     *
     *  // get the 4-momentum of all MC particles 
     *  LoKi::LorentzVector sum7 = 
     *     LoKi::Adder::addMomenta( mcr.begin() , mcr.end()   ) ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02
     *
     *  @see LoKi::LorentzVector 
     *
     *  @param  begin "begin"-iterator for the sequence of objects
     *  @param  end   "end"-iterator for the sequence of objects
     *  @param  result the initial value 
     *  @return sum of 4-momenta of all particles 
     */
    template <class OBJECT>
    inline LoKi::LorentzVector 
    addMomenta
    ( OBJECT              begin                          , 
      OBJECT              end                            , 
      LoKi::LorentzVector result = LoKi::LorentzVector() ) 
    { 
      for ( ; begin != end ; ++begin ) 
      { if ( 0 != (*begin) ) { result += (*begin)->momentum() ; } }
      return result ;
    }
    // ========================================================================    
    /** the simple function which adds a 4-momenta of all (MC)Particles
     *  from a sequence [begin,end).
     *  The actual type of elements is irrelevant, 
     *  The function relies only on existence of mehtod "momentum()"
     *  with a return-value convertible to LoKi::LorentzVector
     *
     *  e.g. eny sequence of objects, convertible to 
     *  "const LHCb::Particle*","const LHCb::MCParticle*" or 
     *  "const HepMC::GenParticle*" could be used.
     *
     *  @code 
     *  const LHCb::Vertex*             vertex = ... ;
     *
     *  // get the 4-mometum of all charged daughter particle at  vertex:
     *  LoKi::LorentzVector sum1 = 
     *     LoKi::Adder::addMomenta
     *       ( vertex->products().begin() , 
     *         vertex->products().end()   , 0 != Q ) ;
     *  
     *  @endcode 
     *
     *  @see LoKi::Cuts::Q 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02
     *
     *  @see LoKi::LorentzVector 
     *
     *  @param  first  "begin"-iterator for the sequence of objects
     *  @param  last   "end"-iterator for the sequence of objects
     *  @param  cut     the predicate 
     *  @param  result  the initial value for result 
     *  @return sum of 4-momenta of all particles 
     */
    template <class OBJECT, class PREDICATE>
    inline LoKi::LorentzVector 
    addMomenta
    ( OBJECT              first                          , 
      OBJECT              last                           , 
      PREDICATE           cut                            , 
      LoKi::LorentzVector result = LoKi::LorentzVector() )
    { 
      for ( ; first != last ; ++first ) 
      { 
        if ( 0 != (*first) && predicate ( *first ) ) 
        { result += (*first)->momentum() ; } 
      }
      return result ;
    }
    // ========================================================================    
    /** the simple function which adds a 4-momenta of all (MC)Particles
     *  fron "container/sequence/range"
     *  The actual type of container is irrelevant it could be, e.g. 
     *    - std::vector<Particle*>
     *    - std::vector<const Particle*>
     *    - std::vector<MCParticle*>
     *    - std::vector<const MCParticle*>
     *    - Particles
     *    - MCParticles
     *    - SmartRefVectro<Particle>
     *    - SmartRefVectro<MCParticle>
     *    - Range 
     *    - MCRange 
     *
     *  @code
     *  
     *  const Vertex* vertex = ... ;
     *  
     *  // get the 4-momentum of all daughter particle at  vertex:
     *  const LoKi::LorentzVector sum = 
     *     LoKi::Adder::addMomenta( vertex->products() ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::LorentzVector 
     *  @see LoKi::Kinematics::addMomenta
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02
     *
     *  @param seq a container of input (MC)Particles 
     *  @return sum of 4-momenta of all particles 
     */
    template <class OBJECTS>
    inline LoKi::LorentzVector addMomenta
    ( const OBJECTS&             seq                            ,
      const LoKi::LorentzVector& result = LoKi::LorentzVector() ) 
    { return addMomenta ( seq.begin() , seq.end() , result ) ; }
    // ========================================================================    
    /** This routine returns the cosine angle theta 
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "Q" (the parent of "D"), 
     *  with respect to "Q"'s fligth direction in "P"'s
     *  (the parent of "Q") rest frame
     * 
     *  \f$ \cos \theta = \frac
     *  { \left(P \cdot D\right)Q^2 - 
     *    \left(P \cdot Q\right)\left(D \cdot Q \right) }
     *  {\sqrt{ \left[ \left( P \cdot Q \right)^2 - Q^2 P^2 \right] 
     *          \left[ \left( D \cdot Q \right)^2 - Q^2 D^2 \right] } } \f$ 
     *  
     *  Note  that the expression has the symmetry: \f$ P \leftrightarrow D \f$ 
     *  
     *  Essentially it is a rewritten EvtDecayAngle(P,Q,D) 
     *  routine from EvtGen package
     *  
     *  @param D 4-momentum of the daughter particle 
     *  @param Q 4-momentum of mother particle 
     *  @param P "rest frame system"
     *  @return cosine of decay angle 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    double decayAngle
    ( const LoKi::LorentzVector& P , 
      const LoKi::LorentzVector& Q ,
      const LoKi::LorentzVector& D ) ;
    // ========================================================================
    /** This routine returns the cosine angle theta 
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "M" (the parent of "D"), 
     *  with respect to the boost direction from 
     *  "M"'s rest frame 
     *
     *  @param D 4-momentum of the daughter particle 
     *  @param M 4-momentum of mother particle 
     *  @return cosine of decay angle 
     *
     *  Clearly it is a variant of 3-argument with the 
     *  P-argument to be of type (0,0,0,E) 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    inline double decayAngle
    ( const LoKi::LorentzVector& D , 
      const LoKi::LorentzVector& M ) 
    {
      return decayAngle ( LoKi::LorentzVector (0,0,0,2*M.E()) , M , D ) ;
    }
    // ========================================================================
    /** This routine returns the cosine angle theta 
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "M" (the parent of "D"), 
     *  with respect to the boost direction from 
     *  "M"'s rest frame 
     *
     *  @param D 4-momentum of the daughter particle 
     *  @param M 4-momentum of mother particle 
     *  @return cosine of decay angle 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    double decayAngle_
    ( const LoKi::LorentzVector& D , 
      const LoKi::LorentzVector& M ) ;
    // ========================================================================    
//     /** This routine evaluates the cosine of "transversity angle", 
//      *  useful e.g. to disantangle the different partial waves in 
//      *  0 -> 1 + 1 decay (e.g. Bs -> J/psi Phi) 
//      *  
//      *  The code is kindly provided by Gerhard Raven 
//      * 
//      *  @param l1 4-vector of the first  particle, e.g. mu+
//      *  @param l2 4-vector of the second particle, e.g. mu- 
//      *  @param p1 4-vector of the third  particle, e.g. K+
//      *  @param p2 4-vector of the fourth particle, e.g. K- 
//      *  @return the cosine of transversity angle 
//      * 
//      *  @see LoKi::LorentzVector.h
//      *
//      *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//      *  @date 2004-12-03
//      */ 
//     double transversityAngle 
//     ( const LoKi::LorentzVector& l1 , 
//       const LoKi::LorentzVector& l2 , 
//       const LoKi::LorentzVector& p1 , 
//       const LoKi::LorentzVector& p2 ) ;
    
//     /** This routine evaluated the angle theta_FB
//      *  used e.g. for evaluation of forward-backward 
//      *  asymmetry for decay B -> K* mu+ mu- 
//      *  The angle calculated is that 
//      *  between between the mu+ and K^*0 momenta 
//      *  in the di-muon rest frame
//      *  
//      *  The code is kindly provided by Helder Lopes 
//      *
//      *  @param  K  4-momenutm of   "K*"
//      *  @param  l1 4-momentum of the first  lepton
//      *  @param  l2 4-momentum of the second lepton
//      *  @return the cosine of theta_FB 
//      * 
//      *  @see LoKi::LorentzVector
//      *
//      *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//      *  @date 2004-12-03
//      */
//     double forwardBackwardAngle
//     ( const LoKi::LorentzVector& K  , 
//       const LoKi::LorentzVector& l1 , 
//       const LoKi::LorentzVector& l2 ) ;
    // ========================================================================
    /** simple function which evaluates the magnitude of 3-momentum 
     *  of particle "v" in the rest system of particle "M" 
     *
     *  \f$ \left|\vec{p}\right| 
     *     \sqrt{  \frac{\left(v\cdotM\right)^2}{M^2} -v^2} \f$
     * 
     *  @attention particle M must be time-like particle: M^2 > 0 !
     *  @param v the vector to be checked 
     *  @param M the defintion of "rest"-system
     *  @return the magnitude of 3D-momentum of v in rest-frame of M 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2008-07-27
     */
    double restMomentum 
    ( const LoKi::LorentzVector& v , 
      const LoKi::LorentzVector& M ) ;
    // ========================================================================
    /** simple function which evaluates the energy
     *  of particle "v" in the rest system of particle "M" 
     * 
     *  \f$ e = \frac{v\cdotM}{\sqrt{M^2}} \f$
     *  
     *  @attention particle M must be time-like particle: M^2 > 0 !
     *  @param v the vector to be checked 
     *  @param M the defintion of "rest"-system
     *  @return the energy of v in rest-frame of M 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    double restEnergy 
    ( const LoKi::LorentzVector& v , 
      const LoKi::LorentzVector& M ) ;
    // ========================================================================
    /** simple function to evaluate the cosine angle between 
     *  two directions (v1 and v2) in the rest system of M 
     *
     *  \f$ \cos\theta = 
     *   \frac{\vec{p}_1\vec{p}_2}{\left|\vec{p}_1\right|
     *                             \left|\vec{p}_2\right|} = 
     *     \frac{1}{\left|\vec{p}_1\right|\left|\vec{p}_2\right|} 
     *     \left( E_1E_2 -\frac{1}{2}
     *   \left(\left(v_1+v_2\right)^2-v_1^2-v_2^2 \righ) \right) \f$ 
     *  
     *  \f$ E_1 E_2 = \frac{ \left(v_1\cdotM\righ)\keft(v_2\cdotM\right)}
     *    {M^2} \f$ and 
     *  \f$ {\left|\vec{p}_1\right|\left|\vec{p}_2\right| = 
     *   \sqrt{ 
     *   \left( \frac{\left(v_1\cdotM\right)^2}{M^2}-v_1^2 \right)   
     *   \left( \frac{\left(v_2\cdotM\right)^2}{M^2}-v_2^2 \right) } \f$ 
     * 
     *  @attention the particle M must be time-like particle: M^2 > 0 ! 
     *  @param v1 the first vector 
     *  @param v2 the last vector 
     *  @param M  the defintion of rest-system
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    double cosThetaRest 
    ( const LoKi::LorentzVector& v1 ,
      const LoKi::LorentzVector& v2 , 
      const LoKi::LorentzVector& M  ) ; 
    // ========================================================================
    /** evaluate the angle \f$\chi\f$ 
     *  beween two decay planes, 
     *  formed by particles v1&v2 and h1&h2 correspondingly. 
     *  The angle is evaluated in the rest frame 
     *  of "mother" particles (defined as v1+v2+h1+h2) 
     *      
     *  The angle is  calculated as the angle between 
     *  two "4-normales" in the rest frame of mother particle.
     *  The "4-normales" are defined as:
     *  \f$ L_1^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}M^{\kappa} \f$ and 
     *  \f$ L_2^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *                h_1^{\lambda}h_2^{\delta}M^{\rho} \f$.
     *
     *  Clearly we can use the general formula: 
     *
     *  \f$ \cos\theta = 
     *     \frac{1}{\left|\vec{L}_1\right|\left|\vec{L}_2\right|} 
     *     \left( E_1E_2 -\frac{1}{2}
     *   \left(\left(L_1+L_2\right)^2-L_1^2-L_2^2 \right) \right) \f$,
     *
     *  however for 4-normales we have in an obvious way
     *  \f$ L_1 \cdot M = L_2 \cdot M = 0 \f$, therefore 
     *  \f$ E1 = E2 = 0 \f$ and 
     *   \f$ \left| \vec{L}_1 \right| = \sqrt{ -L_1^2 } \f$ 
     *   \f$ \left| \vec{L}_2 \right| = \sqrt{ -L_1^2 } \f$, therefore:
     *
     *  \f$ \cos\theta = 
     *     -\frac{1}{2\sqrt{\left[-L_1^2\right]\left[-L_2^2\right]}}
     *   \left(\left(L_1+L_2\right)^2-L_1^2-L_2^2 \right) \right) \f$,
     *
     *  In a similar way:
     *
     *  \f$ \sin\theta = -\frac
     *  {\epsilon_{\mu\nu\lambda\kappa}
     *      L_1^{\mu}L_2^{\nu}(h_1+h_2)^{\lambda}M^{\kappa}}
     *  {\sqrt{ \left( \left(h_1+h_2\right)\cdot M \right)^2 - 
     *       \left(h_1+h_2\right)^2 }} \f$
     *     
     *  The sign for <c>sin</c> is set according to Thomas Blake's code from
     *  P2VVAngleCalculator tool
     *  @see P2VVAngleCalculator 
     *
     *  @attention the function can be rather expensive to 
     *             evaluate, use faster cosDecayAngleChi 
     *  @see LoKi::Kinematics::cosDecayAngleChi 
     *
     *  @param d1 the 1st daughter 
     *  @param d2 the 2nd daughter 
     *  @param h1 the 3rd daughter 
     *  @param h2 the 4th daughter 
     *  @return angle chi 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    double decayAngleChi 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;  
    // ========================================================================
    /** evaluate \f$\cos \chi\f$, where \f$\chi\f$ if the angle  
     *  beween two decay planes, formed by particles d1&d2 
     *  and h1&h2 correspondingly. 
     *  The angle is evaluated in the rest frame 
     *  of "mother" particles (defined as d1+d2+h1+h2) 
     *      
     *  The angle is  calculated as the angle between 
     *  two "4-normales" in the rest frame of mother particle.
     *  The "4-normales" are defined as:
     *  \f$ L_1^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}M^{\kappa} \f$ and 
     *  \f$ L_2^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *                h_1^{\lambda}h_2^{\delta}M^{\rho} \f$.
     *
     *  Clearly we can use the general formula: 
     *
     *  \f$ \cos\theta = 
     *     \frac{1}{\left|\vec{L}_1\right|\left|\vec{L}_2\right|} 
     *     \left( E_1E_2 -\frac{1}{2}
     *   \left(\left(L_1+L_2\right)^2-L_1^2-L_2^2 \right) \right) \f$,
     *
     *  however for 4-normales we have in an obvious way
     *  \f$ L_1 \cdot M = L_2 \cdot M = 0 \f$, therefore 
     *  \f$ E1 = E2 = 0 \f$ and 
     *   \f$ \left| \vec{L}_1 \right| = \sqrt{ -L_1^2 } \f$ 
     *   \f$ \left| \vec{L}_2 \right| = \sqrt{ -L_1^2 } \f$, therefore:
     *
     *  \f$ \cos\theta = 
     *     -\frac{1}{2\sqrt{\left[-L_1^2\right]\left[-L_2^2\right]}}
     *   \left(\left(L_1+L_2\right)^2-L_1^2-L_2^2 \right) \right) \f$,
     *
     *  @param d1 the 1st daughter 
     *  @param d2 the 2nd daughter 
     *  @param h1 the 3rd daughter 
     *  @param h2 the 4th daughter 
     *  @return cos(chi) 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
     double cosDecayAngleChi 
     ( const LoKi::LorentzVector& d1 , 
       const LoKi::LorentzVector& d2 , 
       const LoKi::LorentzVector& h1 , 
       const LoKi::LorentzVector& h2 ) ; 
    // ========================================================================
    /** evaluate the angle \f$\sin\chi\f$, where \f$\chi\f$ is the angle 
     *  beween two decay planes, 
     *  formed by particles v1&v2 and h1&h2 correspondingly. 
     *  The angle is evaluated in the rest frame 
     *  of "mother" particles (defined as v1+v2+h1+h2) 
     *      
     *  The angle is  calculated as the angle between 
     *  two "4-normales" in the rest frame of mother particle.
     *  The "4-normales" are defined here as:
     *  \f$ L_1^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}M^{\kappa} \f$ and 
     *  \f$ L_2^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *                h_1^{\lambda}h_2^{\delta}M^{\rho} \f$.
     *
     *  Clearly we can use the general formula: 
     *
     *  \f$ \cos\theta = 
     *     \frac{1}{\left|\vec{L}_1\right|\left|\vec{L}_2\right|} 
     *     \left( E_1E_2 -\frac{1}{2}
     *   \left(\left(L_1+L_2\right)^2-L_1^2-L_2^2 \right) \right) \f$,
     *
     *  however for 4-normales we have in an obvious way
     *  \f$ L_1 \cdot M = L_2 \cdot M = 0 \f$, therefore 
     *  \f$ E1 = E2 = 0 \f$ and 
     *   \f$ \left| \vec{L}_1 \right| = \sqrt{ -L_1^2 } \f$ 
     *   \f$ \left| \vec{L}_2 \right| = \sqrt{ -L_1^2 } \f$, therefore:
     *
     *  \f$ \sin\theta = -\frac
     *  {\epsilon_{\mu\nu\lambda\kappa}
     *      L_1^{\mu}L_2^{\nu}(h_1+h_2)^{\lambda}M^{\kappa}}
     *  {\sqrt{ \left( \left(h_1+h_2\right)\cdot M \right)^2 - 
     *       \left(h_1+h_2\right)^2 }} \f$
     *     
     *  The sign for <c>sin</c> is set according to Thomas Blake's code from
     *  P2VVAngleCalculator tool
     *  @see P2VVAngleCalculator 
     *
     *  @attention the function can be rather expensive to 
     *             evaluate, use faster cosDecayAngleChi 
     *  @see LoKi::Kinematics::cosDecayAngleChi 
     *
     *  @param d1 the 1st daughter 
     *  @param d2 the 2nd daughter 
     *  @param h1 the 3rd daughter 
     *  @param h2 the 4th daughter 
     *  @return angle chi 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */ 
    double sinDecayAngleChi 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;
    // ========================================================================
  }  // end of namespace Kinematics  
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_KINEMATICS_H
// ============================================================================
