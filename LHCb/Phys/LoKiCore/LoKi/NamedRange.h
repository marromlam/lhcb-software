// $Id: NamedRange.h,v 1.2 2006-02-18 18:06:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/24 09:39:42  ibelyaev
// New Import: the core part of restructurized LoKi project
//
// ============================================================================
#ifndef LOKI_NAMEDRANGE_H 
#define LOKI_NAMEDRANGE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Range.h"
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
  /** @class NamedRange NamedRange.h LoKi/NamedRange.h
   *
   *  New concept of "named" range : range with name 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-11-19
   */
  template <class TYPE>
  class NamedRange_ : public LoKi::Range_<TYPE> 
  {
  public:
    typedef LoKi::Range_<TYPE>  Base ;
  protected:
    typedef NamedRange_<TYPE>   Self ;
  public:
    
    /// default constructor
    NamedRange_() : Base() , m_name() {};
    
    /** Constructor 
     *  @param ibegin  iterator to begin of the sequence
     *  @param iend    iterator to end   of the sequence
     *  @param name    name of the range 
     */
    NamedRange_( typename Base::iterator ibegin    , 
                 typename Base::iterator iend      , 
                 const std::string&      name = "" ) 
      : Base   ( ibegin , iend ) , m_name ( name ) {} ;
    
    /** constructor from the base class 
     *  @param base base objects 
     *  @param name name of the range 
     */
    NamedRange_( const Base&        base      , 
                 const std::string& name = "" ) 
      : Base   ( base ) , m_name ( name ) {};
    
    /** constructor from the base class 
     *  @param base base objects 
     *  @param name name of the range 
     */
    NamedRange_( const typename Base::_Base& base      , 
                 const std::string&          name = "" ) 
      : Base   ( base ) , m_name ( name ) {};
    
    /** constructor from the base class 
     *  @param base base objects 
     *  @param name name of the range 
     */
    NamedRange_( const typename Base::Container& base      , 
                 const std::string&              name = "" ) 
      : Base   ( base ) , m_name ( name ) {};
    
    /* constructor of empty range/sequence
     * @param ibegin  iterator to begin of empty sequence
     *  @param name name of the range 
     */
    NamedRange_( typename Base::iterator ibegin      , 
                 const std::string&      name   = "" ) 
      : Base   ( ibegin , ibegin ) , m_name ( name ) {};
    
    /// destructor
    ~NamedRange_(){};

  public:
    
    /// get the name of the range 
    const std::string& name() const { return m_name ; }
    /// set the name of the range 
    void  setName( const std::string& value ) { m_name = value ; }
    
  private:
    
    // the name of the range 
    std::string m_name ; ///< the name of the range
    
  };

}; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_NAMEDRANGE_H
// ============================================================================
