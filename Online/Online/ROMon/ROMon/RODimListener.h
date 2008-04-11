// $Id: RODimListener.h,v 1.2 2008-04-11 12:11:22 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RODimListener.h,v 1.2 2008-04-11 12:11:22 frankb Exp $
#ifndef ROMON_ROMONDIMLISTENER_H
#define ROMON_ROMONDIMLISTENER_H 1

// C++ include files
#include <map>
#include <string>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class ROMonInfo;

  struct ROUpdateHandler {
    virtual void update() = 0;
  };

  /**@class RODimListener ROMon.h GaudiOnline/ROMon.h
   *
   * Base class to listen to DIM server creation/deletion
   *
   * @author M.Frank
   */
  class RODimListener {
  public:
    struct Item {
      unsigned int id;
      void* object;
      void (*destruct)(void*);
      Item() : id(0), object(0), destruct(0) {}
      Item(void* obj, void(*d)(void*)) : id(0), object(obj), destruct(d) {
      }
      void release() {
	destruct(this);
	this->~Item();
	::free(this);
      }
      template<class T> T* data() {
	return (T*)(((char*)this)+sizeof(Item)); 
      }
      template <class T> static void del(void* p) {
	T* o = (T*)(((char*)p)+sizeof(Item));
	o->~T();
      }
      template <typename T> static Item* create(void* obj) {
	char* p = (char*)malloc(sizeof(Item)+sizeof(T));
	Item* it = new(p) Item(obj, Item::del<T>);
	new(it->data<T>()) T();
	return it;
      }
    };
    typedef std::map<std::string,Item*>   Clients;
  protected:
    /// Map of known clients
    Clients     m_clients;
    /// Pointer to DNS info handler to accept client notifications
    ROMonInfo*  m_dns;
    /// Verbose flag
    bool        m_verbose;
    /// Reference to update handler
    ROUpdateHandler* m_handler;
    
  public:
    /// Standard constructor
    RODimListener(bool verb);
    /// Standard destructor
    virtual ~RODimListener();
    /// Access to client information
    const Clients& clients() const        {  return m_clients; }
    /// Change verbose flag
    void setVerbose(bool val)             {   m_verbose = val; }
    /// Access verbosity flag
    bool verbose() const                  {  return m_verbose; }
    /// Change handler
    void setUpdateHandler(ROUpdateHandler* h)  {   m_handler = h; }
    /// Change handler
    ROUpdateHandler* updateHandler() const{ return m_handler; }
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc) = 0;
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc) = 0;
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONDIMLISTENER_H */

