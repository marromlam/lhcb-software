// $Id: RelationWeightedBase.h,v 1.1 2002-03-18 19:32:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONWeightedBASE_H
#define RELATIONS_RELATIONWeightedBASE_H 1
// Include files
// STD & STL
#include <vector>
#include <functional>
#include <algorithm>
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StatusCode.h"
// Relations
#include "Relations/Relations.h"

namespace Relations
{
  /** @class RelationWeightedBase RelationWeightedBase.h
   *
   *  The useful helper class for effective  implementation
   *  of weighted unidirectional relations
   *  
   *  @warning for the current implementation the actual type of 
   *  "FROM" should differ from the actual type of "TO"
   *  
   *  @warning users should not deal with this class directly!
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   27/01/2002
   */
  template<class FROM, class TO, class WEIGHT>
  class RelationWeightedBase
  {
  public:
    
    /// shortcut for own type
    typedef RelationWeightedBase<FROM,TO,WEIGHT>                  OwnType    ;
    /// shortcut for inverse type
    typedef RelationWeightedBase<TO,FROM,WEIGHT>                  InvType    ;
    /// short cut for type traits
    typedef Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT> TypeTraits ;
    /// actual "FROM" type
    typedef TypeTraits::From                                      From       ;
    /// actual "TO" type
    typedef TypeTraits::To                                        To         ;
    /// actual "Weight" type
    typedef TypeTraits::Weight                                    Weight     ;
    /// Entry type 
    typedef TypeTraits::Entry                                     Entry      ;
    /// container type 
    typedef TypeTraits::Entries                                   Entries    ;
    /// size_type 
    typedef Entries::size_type                                    size_type  ;
    /// iterator type 
    typedef Entries::iterator                                     iterator   ;
    /// iterator type (internal) 
    typedef Entries::iterator                                     IT         ;
    /// iterator pair type (internal) 
    typedef TypeTraits::IP                                        IP         ;
    /// const_iterator type (internal) 
    typedef TypeTraits::CIT                                       CIT        ;
    
  protected:
    
    /// comparison criteria for full ordering
    typedef TypeTraits::Less                                      Less       ;
    /// comparison criteria ( "less" by "From" value) 
    typedef TypeTraits::LessByFrom                                Less1      ;
    /// comparison criteria ( "less" by "Weight" value) 
    typedef TypeTraits::LessByWeight                              Less2      ;
    /// equality criteria   ( "equal" by "To" value)
    typedef TypeTraits::EqualByTo                                 Equal      ;
    
    /** @struct  Comp1
     *  comparison/ordering criteria using "Weight" and "To" fields
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Comp1: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry
       *  @param  entry2 the secons entry
       *  @return true   if "Weight" field of the first entry is less
       *                 and "To" fields are equal!
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return Equal()( entry1 , entry2 ) && Less2()( entry1 , entry2 ) ; };
    };
    
    /** @struct  Comp2
     *  comparison/ordering criteria using "Weight" and "To" fields
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   27/01/2002
     */
    struct Comp2: public std::binary_function<Entry,Entry,bool>
    {
      /** comparison/ordering criteria
       *  @param  entry1 the first entry
       *  @param  entry2 the secons entry
       *  @return true   if "Weight" field of the first entry is less
       *                 and "To" fields are equal!
       */
      inline bool operator() ( const Entry& entry1 ,
                               const Entry& entry2 ) const
      { return Equal() ( entry1 , entry2 ) && !Less2()( entry1 , entry2 ) ; };
    };
    
  public:
    
    /** retrive all relations from the given object
     *
     *    - the CPU performance is proportional to log(N),
     *      where N is the total number of relations
     *
     *  @see    IRelationWeighted
     *  @see    TypeTraits
     *  @param  object  smart reference to the object
     *  @return pair of iterators for output relations
     */
    inline  IP    i_relations
    ( const From& object      ) const
    { return std::equal_range( begin() , end() , Entry( object ) , Less1() );};
    
    /** retrive all relations from the object which has weigth
     *  larger/smaller than the threshold value
     *
     *    - the CPU performance is proportional to log(N) + log(M)
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *  @see    IRelationWeighted
     *  @see    TypeTraits
     *  @param  object     smart reference to the object
     *  @param  threshold  threshold value for the weight
     *  @param  flag       flag for larger/smaller
     *  @return pair of iterators for output relations
     */
    inline  IP      i_relations
    ( const From&   object     ,
      const Weight& threshold  ,
      const bool    flag       ) const
    {
      // find all relations from the given object
      IP ip = i_relations( object );
      // no relations are found !!!
      if( ip.second == ip.first ) { return ip ; }                // RETURN !!!
      // find the appropriate relations
      iterator it = 
        std::lower_bound( ip.first , ip.second ,
                          Entry( object , To() , threshold )  , Less2()  );
      return flag ?  IP( it , ip.second ) : IP( ip.first , it );
    };
    
    /** make the relation between 2 objects
     *
     *    - the CPU performance is proportional to log(N) + M
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *    - StatusCode::FAILURE is returned if the relation
     *      i already set, the existing relation remains untouched
     *
     *  @see    IRelationWeighted
     *  @param  object1 smart reference to the first object
     *  @param  object2 smart reference to the second object
     *  @param  weight  weigth for the relation
     *  @return status  code
     */
    inline StatusCode i_relate
    ( const From&     object1 ,
      const To&       object2 ,
      const Weight&   weight  )
    {
      // get all existing relations from object1
      IP ip = i_relations( object1 ) ;
      // does the given relation between object1 and object1 exist ?
      const Entry entry ( object1 , object2 , weight );
      iterator it = std::find_if( ip.first , ip.second ,
                                  std::bind2nd( Equal() , entry ) );
      if( ip.second != it   ) { return StatusCode::FAILURE ; }     // RETURN !!!
      // find the place where to insert the relation and insert it!
      it = std::lower_bound( ip.first , ip.second , entry , Less2() ) ;
      m_entries.insert( it , Entry( object1 , object2 , weight ) ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** remove the concrete relation between objects
     *
     *    - the CPU performance is proportional to log(N) + M
     *      where N is the total number of relations and
     *      M is the mean relation multiplicity
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see    IRelationWeighted
     *  @param  object1  smart reference to the first object
     *  @param  object2  smart reference to the second object
     *  @return status code
     */
    inline  StatusCode i_remove
    ( const From&      object1 ,
      const To&        object2 )
    {
      // get all existing relations form object1
      IP ip = i_relations( object1 );
      // does the given relation between object1 and object1 exist ?
      iterator it = std::find_if( ip.first , ip.second ,
                                  std::bind2nd( Equal() ,
                                                Entry( object1 , object2 ) ) );
      if( ip.second == it   ) { return StatusCode::FAILURE ; }    // RETURN !!!
      // remove the relation
      m_entries.erase( it );
      return StatusCode::SUCCESS ;
    };
    
    /** remove all relations FROM the defined object
     *
     *    - the CPU performance is proportional to log(N)
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see    IRelationWeighted
     *  @param  object  smart reference to the object
     *  @return status code
     */
    inline  StatusCode i_remove
    ( const From&      object )
    {
      // get all existing relations form object1
      IP ip = i_relations( object );
      // no relations are found !!!
      if( ip.second == ip.first ) { return StatusCode::FAILURE; }// RETURN !!!
      // remove relations
      m_entries.erase( ip.first , ip.second );
      return StatusCode::SUCCESS ;
    };
    
    /** remove all relations TO the defined object
     *
     *    - the CPU performance is proportional to N
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see       IRelationWeighted
     *  @attention the method is relatively slow
     *  @param object  smart reference to the object
     *  @return status code
     */
    inline  StatusCode i_remove
    ( const To&        object )
    {
      iterator it =
        std::remove_if( begin() , end() ,
                        std::bind2nd( Equal() , Entry( From() , object ) ) ) ;
      // no relations are found!
      if( end() == it ) { return StatusCode::FAILURE ; }           // RETURN !!
      // remove relations
      m_entries.erase( it , end() ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** filter out the relations FROM the defined object, which
     *  have a weight larger(smaller)than the threshold weight
     *
     *    - the CPU performance is proportional to log(N) + log(M)
     *      where N is the total number of relations and
     *      M is a mean relation multiplicity
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see    IRelationWeighted
     *  @param  object     smart reference to the object
     *  @param  threshold  threshold value for the weight
     *  @param  flag       flag for larger/smaller
     *  @return status     code
     */
    inline  StatusCode i_filter
    ( const From&      object    ,
      const Weight&    threshold ,
      const bool       flag      )
    {
      // get all relations from the object over/under the threshold
      IP ip = i_relations( object , threshold , !flag );
      // no relations are found!
      if( ip.second == ip.first ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase relations
      m_entries.erase( ip.first , ip.second );
      return StatusCode::SUCCESS ;
    };
    
    /** filter out the relations TO the defined object, which
     *  have a weight larger/smaller than the threshold weight
     *
     *    - the CPU performance is proportional to N
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see       IRelationWeighted
     *  @attention the method is relatively slow
     *  @param     object     smart reference to the object
     *  @param     threshold  threshold value for the weight
     *  @param     flag       flag for larger/smaller
     *  @return    status     code
     */
    inline  StatusCode i_filter
    ( const To&        object    ,
      const Weight&    threshold ,
      const bool       flag      )
    {
      // remove using predicates
      iterator it = flag ?
        std::remove_if( begin() , end  () ,
                        std::bind2nd( Comp1() , Entry( From ()   , 
                                                       object    , 
                                                       threshold ) ) ) :
        std::remove_if( begin() , end() ,
                        std::bind2nd( Comp2() , Entry( From ()   , 
                                                       object    , 
                                                       threshold ) ) ) ;
      // nothing to be removed
      if( end() == it ) { return StatusCode::FAILURE ; }    // RETURN !!!
      // erase the relations
      m_entries.erase( it , end() );
      return StatusCode::SUCCESS ;
    };
    
    /** filter out all relations which
     *  have a weight larger/smaller than the threshold weight
     *
     *    - the CPU performance is proportional to N
     *      where N is the total number of relations
     *
     *    - StatusCode::FAILURE is returned if no appropriate relation
     *      was found
     *
     *  @see       IRelationWeighted
     *  @attention the method  is relatively slow
     *  @param     threshold   threshold value for the weight
     *  @param     flag        flag for larger/smaller
     *  @return    status code
     */
    inline  StatusCode i_filter
    ( const Weight&    threshold ,
      const bool       flag      )
    {
      // remove using the predicates
      iterator it = flag ?
        std::remove_if( begin() , end  () ,
                        std::bind2nd( Less2() ,
                                      Entry( From () ,
                                             To   () , threshold ) ) ) :
        std::remove_if( begin() , end  () ,
                        std::not1( std::bind2nd( Less2() ,
                                                 Entry( From () ,
                                                        To   ()   ,
                                                        threshold ) ) ) ) ;
      // nothing to be removed
      if( end() == it ) { return StatusCode::FAILURE ; }       // RETURN !!!
      // erase the relations
      m_entries.erase( it , end() );
      return StatusCode::SUCCESS ;
    };
    
    /** standard/default constructor
     *  @param reserve size of preallocated reserved space
     */
    RelationWeightedBase( const size_type reserve = 0 )
      : m_entries() 
    { if( reserve ) { Relations::reserve( m_entries , reserve ) ; } };
    
    /// destructor (virtual)
    virtual ~RelationWeightedBase()
    { m_entries.clear(); }
    
    /** constructor from inverse relation table
     *  @attention it is the way to invert existing relations!
     *  @param inv relations to be inverted
     */
    RelationWeightedBase( const InvType& inv )
      : m_entries()
    {
      // reserve the container size to avoid the relocations
      m_entries.reserve( inv.i_entries().size() );
      // invert the relations
      for( InvType::CIT it = inv.begin() ; inv.end() != it ; ++it )
        { i_relate( it->to() , it->from() , it->weight() ) ;  }
    };
    
    /** assignement operator
     *  @param copy object to be copied
     */
    OwnType& operator=( const OwnType& copy )
    {
      if( &copy == this ) { return *this ; }
      m_entries = copy.m_entries;
      return *this;
    };
    
    inline iterator begin ()        { return m_entries.begin () ; }
    inline iterator begin ()  const { return m_entries.begin () ; }
    inline iterator end   ()        { return m_entries.end   () ; }
    inline iterator end   ()  const { return m_entries.end   () ; }
    
    /** accessor to the container of entries
     *  @return reference to the whole store of relations
     */
    inline       Entries& i_entries ()        { return m_entries ; }
    
    /** accessor to the container of entries (const version)
     *  @return reference to the whole store of relations
     */
    inline const Entries& i_entries () const  { return m_entries ; }
    
  private:
    
    mutable Entries m_entries ;
    
  };
  
}; //  end of namespace Relations


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONWeightedBASE_H
// ============================================================================
