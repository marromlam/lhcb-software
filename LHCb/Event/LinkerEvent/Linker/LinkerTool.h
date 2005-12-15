// $Id: LinkerTool.h,v 1.4 2005-12-15 10:00:32 ocallot Exp $
#ifndef LINKER_LINKERTOOL_H 
#define LINKER_LINKERTOOL_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ObjectVector.h"

#include "Event/LinksByKey.h"

#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTable.h"

/** @class LinkerTool LinkerTool.h Linker/LinkerTool.h
 *  Mimic the relation tool as much as possible
 *
 *  @author Olivier Callot
 *  @date   2005-01-19
 */
template <class SOURCE,
          class TARGET,
          class SOURCECONTAINER = ObjectVector<SOURCE>,
          class TARGETCONTAINER = ObjectVector<TARGET> >
class LinkerTool {

public: 

  typedef LinkerTable<SOURCE,TARGET,SOURCECONTAINER,TARGETCONTAINER>  DirectType;
  typedef LinkerTable<TARGET,SOURCE,TARGETCONTAINER,SOURCECONTAINER>  InverseType;
  typedef typename DirectType::Range                                  Range;
  typedef typename DirectType::iterator                               iterator;
  friend class DirectType;

  /** Standard constructor
   *  @param  svc             event service
   *  @param  containerName   name of the container of links.
   */
  LinkerTool( IDataProviderSvc* svc, std::string containerName ) {
    m_evtSvc = svc;
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }
    m_location    = name;
    m_invLocation = name + "Inv";
  };

  ~LinkerTool( ) {}; ///< Destructor

  /** retrieve the direct relation
   *  @return     The direct table of relation.
   */

  DirectType* direct ( ) {
    SmartDataPtr<LHCb::LinksByKey> links( m_evtSvc, m_location );
    if ( 0 != links ) links->resolveLinks( m_evtSvc );
    const LHCb::LinksByKey* linkPtr = links;
    m_table.load( linkPtr );
    if ( 0 == linkPtr ) return 0;
    return &m_table;
  }

  /** retrieve the inverse relation, build it if not yet done
   *  @return     The inverse table of relation.
   */

  InverseType* inverse ( ) {
    SmartDataPtr<LHCb::LinksByKey> links( m_evtSvc, m_invLocation );
    LHCb::LinksByKey* linkPtr = links;
    if ( 0 == linkPtr ) {
      //== Invert the table...
      const DirectType* tmp = direct();
      if ( 0 != tmp ) {
        // Create, with name shortened to remove the "Link/" prefix
        LinkerWithKey<SOURCE,TARGET> makeLink( m_evtSvc, 0,
                                               m_invLocation.substr(5) );
        Range rd = tmp->relations();
        for ( iterator it = rd.begin(); rd.end() != it; ++it ) {
          makeLink.link( it->to(), it->from(), it->weight() );
        }
        SmartDataPtr<LHCb::LinksByKey> newLinks( m_evtSvc, m_invLocation );
        linkPtr = newLinks;
      }
    }
    if ( 0 != linkPtr ) linkPtr->resolveLinks( m_evtSvc );
    m_invTable.load( linkPtr );
    if ( 0 == linkPtr ) return 0;
    return &m_invTable;
  }

protected:

private:
  IDataProviderSvc* m_evtSvc;
  std::string       m_location;
  std::string       m_invLocation;
  DirectType        m_table;
  InverseType       m_invTable;
};
#endif // LINKER_LINKERTOOL_H
