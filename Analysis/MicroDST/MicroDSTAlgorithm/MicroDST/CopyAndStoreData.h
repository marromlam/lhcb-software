// $Id: CopyAndStoreData.h,v 1.4 2007-10-16 14:08:54 jpalac Exp $
#ifndef COPYANDSTOREDATA_H 
#define COPYANDSTOREDATA_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/IRegistry.h>

class ObjectContainerBase;
/** @class CopyAndStoreData CopyAndStoreData.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-11
 */
class CopyAndStoreData : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CopyAndStoreData( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyAndStoreData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_inputTESLocation;
  std::string m_outputPrefix;
  std::string m_fullOutputTESLocation;

protected:

  /**
   * Copy an object of type T from the TES onto another TES location.
   * Uses the copy constructor of type T for copying.
   * 
   * @param from The TES location of the object to be copied
   * @param to   The TES location where the copy should be stored
   * @return     Status code
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @author Ulrich Kerzel
   */
  template <class T>
  StatusCode copyAndStoreObject( const std::string& from,
                                 const std::string& to   ) 
  {
    verbose() << "try to get data container" << endmsg;

    if (exist<T>( from ) ) {
      const T* data = get<T>( from );
      verbose() << "now copy information" << endmsg;
      T* newData = new T(*data);
      put (newData, to );
      verbose() << "Data values set to\n" << *newData << "\n" << endmsg;
    } else {
      Warning("No data container found at "+ from, StatusCode::SUCCESS);
    } // if exist

    return StatusCode::SUCCESS;
  }
  //===========================================================================

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class containerCloner >
  StatusCode copyAndStoreContainer( const std::string& from,
                                    const std::string& to,
                                    const containerCloner& cloner) 
  {
    debug() << "now store container for location " << from << endmsg;

    if (!exist<T>(from)) {
      debug() << "Container location does not exist" << endmsg;
      return StatusCode::FAILURE;    
    } else {
      const T* data = get<T>( from );
      if (!data) {
        return StatusCode::FAILURE;
      }
      verbose() << "got # elements in container: " << data->size() << endmsg;
      T* clones = getOutputContainer<T>(to);
      return cloner.clone(data, clones);
    } // if !exist
  }
  //===========================================================================
  /**
   *
   * @author Juan Palaicos juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class T, class itemCloner>
  StatusCode cloneItemIntoContainer(const T* item,
                                    const std::string& to,
                                    const itemCloner& cloner) 
  {
    typename T::Container* container = getOutputContainer<T::Container>(to);
    if ( !container->object( item->key() ) ) {
      T* clonedItem = cloner.clone(item);
      container->insert( clonedItem, item->key()) ;
    }
    return StatusCode::SUCCESS;
  }
  //===========================================================================

  /**
   * Clone the contents of a container and put into another one.
   * Designed for keyed containers.
   * Contained types must have a clone() method.
   * Container must have an object(key), an insert and a 
   * value_type typedef.
   * The functor checks if an object of type T::value_type already
   * exists in "clones". If not, the object is cloned and inserted.
   *
   * @author Juan Palacios juanch@nikhef.nl
   * @date 15-10-2007
   */
  template <class T, class itemCloner> 
  struct ContainerCloner 
  {
  public:
    explicit ContainerCloner(const itemCloner& cloner)
      : m_cloner(cloner)
    {
    }
    
    StatusCode clone(const T* data, T* clones) const
      {
        for (typename T::const_iterator i = data->begin(); 
             i != data->end(); 
             ++i) {
          //          typename T::value_type item = clones->object( (*i)->key() );
          if ( !clones->object( (*i)->key() ) ) {
             typename T::value_type item = cloner().clone(*i);
            clones->insert(item, (*i)->key());
          }
        }
        return StatusCode::SUCCESS;
      }

    inline const itemCloner& cloner() const
    {
      return m_cloner;
    }
    inline itemCloner& cloner()
    {
      return m_cloner;
    }

  private:
    itemCloner m_cloner;
    
  };
  
  //===========================================================================

  /**
   */
  template <class T>
  class BasicItemCloner 
  {
  public:
    T* clone(const T* item) 
    {
      return item->clone();
    }
  };
  
  //===========================================================================

  /**
   * Get the TES container in the TES location to be stored on the
   * MicroDST. 
   * @param locTES The address in the original TES
   * @return pointer to the container in the MicroDST TES
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getOutputContainer( const std::string& location ) 
  {
    if ( !exist<T>( location ) ) {
      T* container = new T();
      put(container, location);
    }
    return get<T>( location );     
  }
  //=========================================================================  

  /**
   * Get the TES location of an object.
   * 
   * @param pObject Pointer to he onject on the TES
   * @param it's location, if available, otherwise meaningful error names.
   *
   * @author Ulrich Kerzel
   *
   */
  inline const std::string objectLocation(const DataObject* pObject) const
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

  inline const std::string& inputTESLocation() const 
  {
    return m_inputTESLocation;
  }

  inline std::string& inputTESLocation()
  {
    return m_inputTESLocation;
  }

  inline const std::string& outputPrefix() const 
  {
    return m_outputPrefix;
  }

  inline const std::string& fullOutputTESLocation() const 
  {
    return m_fullOutputTESLocation;
  }

  inline std::string& fullOutputTESLocation() 
  {
    return m_fullOutputTESLocation;
  }

  inline void setInputTESLocation(const std::string& newLocation) 
  {
    this->inputTESLocation() = newLocation;
    setFullOutputTESLocation();
  }

  inline void setFullOutputTESLocation() 
  {
    this->fullOutputTESLocation() = "/Event/"+ this->outputPrefix() + 
      "/" + this->inputTESLocation();
  }

  inline const std::string outputTESLocation(std::string& inputLocation) 
  {
    getNiceLocationName(inputLocation);
    return "/Event/"+ this->outputPrefix() + inputLocation;
  }
  
private:

  /**
   * Most orignial locations start with /Event/<alg,Phys,MC, ..>/...
   *  - but not all, "/Event" is optional, some have it, some don't
   * insert the output-prefix to distinguish the microDST location
   * -> remove the "/Event" bit if found at beginning of routine
   * 
   * @author Ulrich kerzel
  */
  void getNiceLocationName(std::string& location);
  
};
#endif // COPYANDSTOREDATA_H
