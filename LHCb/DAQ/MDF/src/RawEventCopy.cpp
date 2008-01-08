// $Id: RawEventCopy.cpp,v 1.5 2008-01-08 15:20:37 cattanem Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "MDF/RawEventHelpers.h"
#include "Event/RawEvent.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {
class RawEvent;

  /** @class RawEventCopy RawEventCopy.cpp
    *
    *  Creates and copies RawEvent data structure to new location
    *  in the data store.
    *
    *  @author Markus Frank
    *  @date   2005-10-13
    */
  class RawEventCopy : public Algorithm {
    /// Property: source leaf for rawevent copy
    std::string m_source;
    /// Property: target leaf name for rawevent copy
    std::string m_destination;

  public: 
    /// Standard constructor
    RawEventCopy(const std::string& nam, ISvcLocator* pSvc) 
    : Algorithm(nam,pSvc) {
      declareProperty("Destination",m_destination="/Event/DAQ/RawCopy");
      declareProperty("Source",     m_source=RawEventLocation::Default);
    }
    /// Destructor
    virtual ~RawEventCopy()  {} 
    /// Main execution
    virtual StatusCode execute()  {
      RawEvent *org = 0, *res = 0;
      StatusCode sc = eventSvc()->retrieveObject(m_source,(DataObject*&)org);
      if ( sc.isSuccess() )  {
        sc = cloneRawEvent(org, res);
        if ( sc.isSuccess() )  {
          sc = eventSvc()->registerObject(m_destination, res);
          if ( !sc.isSuccess() )  {
            delete res;
          }
        }
      }
      return sc;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventCopy)
