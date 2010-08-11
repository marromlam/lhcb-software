// $Id: Functors.hpp,v 1.8 2010-08-11 12:40:23 jpalac Exp $
#ifndef MICRODST_FUNCTORS_HPP 
#define MICRODST_FUNCTORS_HPP 1

// Include files
#include "MicroDST/Functions.h"
/** @namespace MicroDST Functors.hpp MicroDST/Functors.hpp
 *  
 *
 *  Collection of useful functors satisfying the Cloner policy and
 *  dealing with the cloning and storing clones into TES locations.
 *
 *  Policy: Cloner must export a clone(const T*) method that returns
 *          a T*. If the input T* is null, Cloner must return a null
 *          pointer.
 *          A cloner must export member type T as ::Type.
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-24
 */
namespace MicroDST {

  /**
   *
   * Clone an item into a container of type T. 
   * The functor checks if an item with the same key already exists and only
   * clones and inserts the clone into the container if this is not the case.
   *
   * Requirements:
   *
   * Contained type T:
   * Must export container type as T::Container. 
   * T::Container must have a method 
   * insert(T::value_type, type of T::value_type::key() )
   * Must have a method T* object( type of T::key() )
   *
   * Type T must have method T::key()
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class itemCloner>
  struct CloneKeyedContainerItem
  {
    typedef typename itemCloner::Type Type;

    CloneKeyedContainerItem(typename Type::Container*& to) 
      : m_to(to) { }
    
    Type* operator() ( const Type* item )
    {
      return clone(item);
    }

    Type* clone( const Type* item ) 
    {
      if (0==item) return 0;
      if ( !m_to->object( item->key() ) ) {
	Type* clonedItem = itemCloner::clone(item);
	if (0!=clonedItem) m_to->insert( clonedItem, item->key()) ;
	return clonedItem;
      }
      return m_to->object( item->key() );
    }

    typename itemCloner::Type::Container* m_to;

  };


  /**
   *
   * BasicItemCloner satisfying the Cloner policy. 
   * Requirements on template parameter T:
   * T must export a method T* T::clone().
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */

  template <class T>
  struct BasicItemCloner 
  {
    typedef T Type;
    Type* operator () (const Type* item)    { return clone(item);   }
    static Type* copy(const Type* item) { return clone(item); }
    static Type* clone(const Type* item) { return item ? item->clone() : 0; }
  };

  /**
   *
   * BasicCopy satisfying the Cloner policy. 
   * Requirements on template parameter T:
   * T must have a copy constructor.
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class T>
  struct BasicCopy 
  {
    typedef T Type;
    Type* operator () (const Type* item)    { return copy(item);   }
    static Type* copy(const Type* item) { return item ? new Type(*item) : 0; }
    static Type* clone(const Type* item) { return copy(item); }
  };
  //===========================================================================


}
#endif // MICRODST_FUNCTORS_HPP
