#include "Kernel/IANNSvc.h"
#include "Kernel/IPropertyConfigSvc.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h" 

#include "Event/RawEvent.h"

#include "Kernel/TCK.h"

using std::map;
using std::vector;
using std::string;
using boost::optional;

using namespace LHCb;

class ANNDispatchSvc : public Service 
                     , virtual public IANNSvc 
                     , virtual public IIncidentListener {

public:
  ANNDispatchSvc( const std::string& name, ISvcLocator* pSvcLocator);
  ~ANNDispatchSvc() {}
  
  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  StatusCode initialize(  );
  StatusCode finalize();

  // IncidentListener interface
  void handle(const Incident& incident);

  // IANNSvc interface: delegate to child...

  boost::optional<minor_value_type>  value(const major_key_type& major, const std::string& minor) const
  { verify(); return m_child->value(major,minor); }
  boost::optional<minor_value_type>  value(const major_key_type& major, int minor) const
  { verify(); return m_child->value(major,minor); }

  bool                           hasMajor(const major_key_type& major) const
  { verify(); return m_child->hasMajor(major); }

  std::vector<minor_key_type>    keys(const major_key_type& major) const
  { verify(); return m_child->keys(major); }
  std::vector<minor_value_type>  items(const major_key_type& major) const
  { verify(); return m_child->items(major); }
  std::vector<major_key_type>    majors() const
  {  verify(); return m_child->majors(); }
private:
  void verify() const { if (!m_uptodate) faultHandler(); }
  void faultHandler() const ;


  mutable IDataProviderSvc* m_evtSvc;
  mutable IANNSvc         * m_child;
  mutable IIncidentSvc    * m_incidentSvc;
  mutable IPropertyConfigSvc * m_propertyConfigSvc;
  mutable bool m_uptodate;
  std::string       m_propertyConfigSvcName;
  std::string       m_instanceName;
  std::string       m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;
  mutable unsigned int      m_currentTCK;
  mutable PropertyConfig::digest_type m_currentDigest;
};

#include "GaudiKernel/SvcFactory.h"
DECLARE_SERVICE_FACTORY( ANNDispatchSvc );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ANNDispatchSvc::ANNDispatchSvc( const string& name, ISvcLocator* pSvcLocator)
  : Service( name , pSvcLocator )
  , m_evtSvc(0)
  , m_child(0)
  , m_incidentSvc(0)
  , m_propertyConfigSvc(0)
  , m_uptodate(false)
  , m_inputRawEventLocation("")
{
  declareProperty("IANNSvcInstance", m_instanceName = "HltANNSvc");
  declareProperty("IPropertyConfigSvcInstance", m_propertyConfigSvcName = "PropertyConfigSvc");
  declareProperty("RawEventLocation", m_inputRawEventLocation); 

  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);

}

StatusCode
ANNDispatchSvc::initialize(  )
{

  StatusCode sc = Service::initialize();

  if (sc.isFailure()) {
    fatal() << "Service::initialize() failed!!!"<< endmsg;
    return sc;
  }

  verbose() << "==> Initialize" << endmsg;

  if (!service("EventDataSvc", m_evtSvc).isSuccess()) { 
    fatal() << "ANNDispatchSvc failed to get the EventDataSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // grab IncidentSvc
  if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) {
    fatal() << "ANNDispatchSvc failed to get the IncidentSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // grab IConfigAccessSvc
  if (!service( m_propertyConfigSvcName, m_propertyConfigSvc).isSuccess()) {
    fatal() << "ANNDispatchSvc failed to get the IConfigAccessSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // grab IANNSvc
  if (!service( m_instanceName, m_child).isSuccess()) {
    fatal() << "ANNDispatchSvc failed to get the IANNSvc." << endmsg;
    return StatusCode::FAILURE;
  }

  // add listener for beginEvent... (should be beginRun, but that doesn't seem to be fired...)
  bool rethrow = false;
  bool oneShot = false;
  m_incidentSvc->addListener(this,IncidentType::BeginEvent,int(0),rethrow,oneShot);
  // incidentSvc->addListener(this,IncidentType::BeginRun,priority,rethrow,oneShot);
  m_uptodate = false;
  return sc;
}

StatusCode
ANNDispatchSvc::finalize(  )
{
  StatusCode status = Service::finalize();
  if (m_propertyConfigSvc) { m_propertyConfigSvc->release(); m_propertyConfigSvc=0; }
  if (m_incidentSvc) { m_incidentSvc->release(); m_incidentSvc=0; }
  if (m_evtSvc) { m_evtSvc->release(); m_evtSvc=0; }
  if (m_child) { m_child->release(); m_child=0; }
  return status;
}
//=============================================================================
// Incident handler
//=============================================================================
void ANNDispatchSvc::handle(const Incident& /*incident*/) {
  m_uptodate = false;
}

//=============================================================================
// update child
//=============================================================================

void ANNDispatchSvc::faultHandler() const {

  //Get the Hlt DecReports
  //SmartDataPtr<LHCb::HltDecReports> decReports(m_evtSvc, LHCb::HltDecReportsLocation::Default );
  //Get the TCK from the DecReports
  //unsigned int TCK = decReports->configuredTCK();  
    
  //Decode the raw event to get the TCK from the raw Hlt DecReports
  unsigned int tck = 0;
  SmartDataPtr<LHCb::RawEvent> rawEvent(m_evtSvc, m_inputRawEventLocation); 
  std::vector<std::string>::const_iterator iLoc = m_rawEventLocations.begin();
  for (; iLoc != m_rawEventLocations.end() && rawEvent==0 ; ++iLoc ) {
    rawEvent = SmartDataPtr<LHCb::RawEvent>(m_evtSvc, *iLoc); 
  }

  if (!rawEvent) {
    verbose() << " No RawEvent found! We will get ANN info from HltInit instead." << endmsg;
    m_uptodate = true;
    m_currentTCK = tck;
    return;    
  }
  //Now we get the TCK only from the raw event
  const std::vector<RawBank*> hltdecreportsRawBanks = rawEvent->banks( RawBank::HltDecReports );
  if( hltdecreportsRawBanks.size() == 0) {
    verbose() << " No HltDecReports RawBank in RawEvent. We will get ANN info from HltInit instead." << endmsg;
    m_uptodate = true;
    m_currentTCK = tck;
    return; 
  } else {
    const RawBank* hltdecreportsRawBank = hltdecreportsRawBanks.front();
    if (!hltdecreportsRawBank) {
      verbose() << "Corrupted HltDecReport in the RawBank, we will get ANN info from HltInit instead" << endmsg; 
      m_uptodate = true;
      m_currentTCK = tck;
      return; 
    } else {
      const unsigned int *content = hltdecreportsRawBank->begin<unsigned int>();
      // version 0 has only decreps, version 1 has TCK, taskID, then decreps...
      if (hltdecreportsRawBank->version() > 0 ) {
        tck = *content++ ;
      }
    }
  }
  if (tck == 0) {
    // if there is no TCK, do not dispatch
    verbose() << "No TCK was found in the RawBank, we will get ANN info from HltInit instead" << endmsg;
    m_uptodate = true;
    m_currentTCK = tck;
    return;
  }
  if (tck!=m_currentTCK || !m_currentDigest.valid()) {
    verbose() << "Entering this loop" << endmsg;
    TCK _tck(tck); _tck.normalize();
    ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  _tck.str()  );
    // grab properties of child from config database...
    const ConfigTreeNode* tree = m_propertyConfigSvc->resolveConfigTreeNode(alias);
    if (!tree) {
      //If we could not resolve the (non-zero) TCK we have a problem
      verbose() << "Obtained TCK " << _tck << 
        " from the Hlt DecReports which could not be resolved. We will get ANN info from HltInit instead." << endmsg;
      m_uptodate = true;
      m_currentTCK = tck;
      return;
    } else { 
      PropertyConfig::digest_type child = m_propertyConfigSvc->findInTree(tree->digest(), m_instanceName);
      if (!m_currentDigest.valid() || m_currentDigest!=child) {
        const PropertyConfig *config = m_propertyConfigSvc->resolvePropertyConfig(child);
        assert(config!=0);
        // if ( config==0 ) return StatusCode::FAILURE;
        // push properties to child
        SmartIF<IProperty> iProp(m_child);
        for (PropertyConfig::Properties::const_iterator i =  config->properties().begin();i!= config->properties().end(); ++i ) {
          iProp->setProperty( i->first, i->second  );
        }
        m_currentDigest = child;
        // do not reinit for ANNSvc derived instances, as they have a proper updateHandler...
        // StatusCode sc = m_child->reinitialize();
      }
    }
  }
  m_uptodate = true;
  m_currentTCK = tck;

}



//=============================================================================
// queryInterface
//=============================================================================
StatusCode ANNDispatchSvc::queryInterface(const InterfaceID& riid,
                                          void** ppvUnknown) {
  if ( IANNSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = (IANNSvc*)this;
    addRef();
    return SUCCESS;
  }
  if ( IANSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = (IANSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}
