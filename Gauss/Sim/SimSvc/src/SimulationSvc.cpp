// $ID: $

// Include Files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include <dom/DOM_Element.hpp>

#include "DetDesc/IXmlSvc.h"
#include <DetDesc/ILVolume.h>

#include "SimulationSvc.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static SvcFactory<SimulationSvc> xmlparsersvc_factory;
const ISvcFactory& SimulationSvcFactory = xmlparsersvc_factory;


// -----------------------------------------------------------------------
// build a standard string from a DOMString
// -----------------------------------------------------------------------
const std::string dom2Std (DOMString domString) {
  char *cString = domString.transcode();
  std::string stdString;
  if (cString) {
    stdString = cString;
    delete [] (cString);
  }
  return stdString;
}


// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
SimulationSvc::SimulationSvc (const std::string& name, ISvcLocator* svc) :
  Service (name, svc) {
  declareProperty ("SimulationDbLocation", m_simDbLocation="empty" );
}

SimulationSvc::~SimulationSvc() 
{
  for (AttributeSet::iterator it=m_attributeSet.begin();
       it!= m_attributeSet.end();it++)  delete (*it).second;
  m_attributeSet.clear();

  for (Dict::iterator itt=m_logvol2Sim.begin();
       itt!= m_logvol2Sim.end();itt++)  delete (*itt).second;
  m_logvol2Sim.clear();

  m_regionsDefs.clear();
}

// -----------------------------------------------------------------------
// Initialize the service.
// -----------------------------------------------------------------------
StatusCode SimulationSvc::initialize() {
  // Before anything we have to initialize grand mother
  StatusCode status = Service::initialize();
  if (!status.isSuccess()) {
    return status;  
  }
  if( m_simDbLocation.empty() || "empty" == m_simDbLocation ) { 
    if ( 0 != getenv("XMLDDDBROOT") ) {
      m_simDbLocation  = getenv("XMLDDDBROOT");
      m_simDbLocation += "/DDDB/Simulation/simulation.xml";
    } else {
      m_simDbLocation = "";
    }
  }
  // loads the color set
  reload();
  // returns
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------
// clear
// ------------------------------------------------------------------------
void SimulationSvc::clear () {
  m_attributeSet.clear();
  m_logvol2Sim.clear();
  m_regionsDefs.clear();
}

// -----------------------------------------------------------------------
// reload
// ------------------------------------------------------------------------
void SimulationSvc::reload () {
  MsgStream log (msgSvc(), "SimulationSvc");

  // erases the old set of attributes
  clear();

  // gets the XmlSvc
  IXmlSvc* xmlSvc;
  StatusCode status = serviceLocator()->service("XmlCnvSvc", xmlSvc, true);
  if (status.isFailure()) {
    log << MSG::ERROR << "Unable to get XmlCnvSvc. The simulation "
        << "attributes will not be loaded." << endreq;
    return;
  }

  log << MSG::INFO << "Loading simulation attributes file \""
      << m_simDbLocation << "\" ..." <<endreq;

  // parses the file containing the simatt definitions
  DOM_Document document = xmlSvc->parse (m_simDbLocation.c_str());
  if (document.isNull()) {
    log << MSG::ERROR << "Unable to parse file " << m_simDbLocation
        << ". The simulation attributes will not be loaded." << endreq;
    return;
  }

  // go through the tree of elements and fill in the attribute sets
  DOM_NodeList domAttrList = document.getElementsByTagName("SimAtt");
  unsigned int i;
  for (i = 0; i < domAttrList.getLength(); i++) {
    DOM_Node attrNode = domAttrList.item(i);
    DOM_Element attr = (DOM_Element&) attrNode;
    std::string name = dom2Std (attr.getAttribute ("name"));
    std::string mASAttribute = dom2Std (attr.getAttribute ("maxAllowedStep"));
    std::string mTRAttribute = dom2Std (attr.getAttribute ("maxTrackLength"));
    std::string mTAttribute = dom2Std (attr.getAttribute ("maxTime"));
    std::string mEkAttribute = dom2Std (attr.getAttribute ("minEkine"));
    std::string mRAttribute = dom2Std (attr.getAttribute ("minRange"));

    // computes the values
    double maxAllowedStep = -1.0;
    double maxTrackLength = -1.0;
    double maxTime = -1.0;
    double minEkine = -1.0;
    double minRange = -1.0;
    if (!mASAttribute.empty()) {
      maxAllowedStep = xmlSvc->eval(mASAttribute, false);
    }
    if (!mTRAttribute.empty()) {
      maxTrackLength = xmlSvc->eval(mTRAttribute, true);
    }
    if (!mTAttribute.empty()) {
      maxTime = xmlSvc->eval(mTAttribute, true);
    }
    if (!mEkAttribute.empty()) {
      minEkine = xmlSvc->eval(mEkAttribute, true);
      }
    if (!mRAttribute.empty()) {
      minRange = xmlSvc->eval(mRAttribute, true);
    }
    
    // creates the attribute and register it
    m_attributeSet[name] =
      new SimAttribute
      (maxAllowedStep, maxTrackLength, maxTime, minEkine, minRange);
  }

  // go through the tree of elements and fill in the logvol2Sim map
  DOM_NodeList domLogvolsList = document.getElementsByTagName("LogVols");
  if (domLogvolsList.getLength() > 0) {
    DOM_Node logvolsNode = domLogvolsList.item(0);
    DOM_Element logvolsElement = (DOM_Element&) logvolsNode;

    DOM_NodeList domLogvolList = logvolsElement.getElementsByTagName("Item");
    unsigned int i;
    for (i = 0; i < domLogvolList.getLength(); i++) {
      DOM_Node logvolNode = domLogvolList.item(i);
      DOM_Element logvol = (DOM_Element&) logvolNode;
      std::string name = dom2Std (logvol.getAttribute ("name"));

      PartAttr* partattr = new PartAttr();
      m_logvol2Sim[name]=partattr;

      DOM_NodeList domLogvolNode = logvol.getElementsByTagName("Cut");
      unsigned int j;
      for (j = 0; j < domLogvolNode.getLength(); j++) {
        DOM_Node cutNode = domLogvolNode.item(j);
        DOM_Element cut = (DOM_Element&) cutNode;
        
        int particle = (int) xmlSvc->eval
          (dom2Std (cut.getAttribute ("particle")), false);
        std::string attr = dom2Std (cut.getAttribute ("attr"));
        // register the association
        partattr->operator[](particle) = m_attributeSet[attr];
      }
    }
  }

  ///////////////////////////////////////////////////////////
  // the part below deals with the production cuts per region

  //create a temporary map to store ProductionCut definitions
  map<std::string, Prcuts> regcut;

  // go through the tree of elements and fill in the ProductionCut sets
  DOM_NodeList domPrCutList = document.getElementsByTagName("ProductionCut");
  unsigned int ii;
  for (ii = 0; ii < domPrCutList.getLength(); ii++) {
    DOM_Node prcutNode = domPrCutList.item(ii);
    DOM_Element prcut = (DOM_Element&) prcutNode;
    std::string name = dom2Std (prcut.getAttribute ("name"));
    std::string atrgammacut = dom2Std (prcut.getAttribute ("gammaCut"));
    std::string atrelectroncut = dom2Std (prcut.getAttribute ("electronCut"));
    std::string atrpositroncut = dom2Std (prcut.getAttribute ("positronCut"));
    std::string atrprotoncut = dom2Std (prcut.getAttribute ("protonCut"));
    std::string atraprotoncut = dom2Std (prcut.getAttribute ("antiProtonCut"));
    std::string atrneutroncut = dom2Std (prcut.getAttribute ("neutronCut"));
    std::string atraneutroncut = dom2Std (prcut.getAttribute ("antiNeutronCut"));

    Prcuts tempcuts;

    // set default values
    tempcuts.gammacut = -1.0;
    tempcuts.electroncut = -1.0;
    tempcuts.positroncut = -1.0;
    tempcuts.protoncut = -1.0;
    tempcuts.aprotoncut = -1.0;
    tempcuts.neutroncut = -1.0;
    tempcuts.aneutroncut = -1.0;
    // get values (if any)

    if (!atrgammacut.empty()) {
      tempcuts.gammacut = xmlSvc->eval(atrgammacut, false);
    }
    if (!atrelectroncut.empty()) {
      tempcuts.electroncut = xmlSvc->eval(atrelectroncut, true);
    }
    if (!atrpositroncut.empty()) {
      tempcuts.positroncut = xmlSvc->eval(atrpositroncut, true);
    }
    if (!atrprotoncut.empty()) {
      tempcuts.protoncut = xmlSvc->eval(atrprotoncut, true);
      }
    if (!atraprotoncut.empty()) {
      tempcuts.aprotoncut = xmlSvc->eval(atraprotoncut, true);
    }
    if (!atrneutroncut.empty()) {
      tempcuts.neutroncut = xmlSvc->eval(atrneutroncut, true);
      }
    if (!atraneutroncut.empty()) {
      tempcuts.aneutroncut = xmlSvc->eval(atraneutroncut, true);
    }
    
    // fill in the temporary map 
    regcut[name] = tempcuts;
  }

  // go through the tree of elements 
  DOM_NodeList domRegionsList = document.getElementsByTagName("Regions");
  if (domRegionsList.getLength() > 0) {
    DOM_Node regionsNode = domRegionsList.item(0);
    DOM_Element regionsElement = (DOM_Element&) regionsNode;

    DOM_NodeList domRegionList = regionsElement.getElementsByTagName("Region");
    unsigned int i;
    for (i = 0; i < domRegionList.getLength(); i++) {
      DOM_Node regionNode = domRegionList.item(i);
      DOM_Element region = (DOM_Element&) regionNode;
      std::string regname = dom2Std (region.getAttribute ("name"));
      std::string prcut = dom2Std (region.getAttribute ("prodcut"));
      
      DOM_NodeList domRegionNode = region.getElementsByTagName("Volume");
      unsigned int j;
      std::vector<std::string> volvect;
      
      for (j = 0; j < domRegionNode.getLength(); j++) 
        {
        DOM_Node volNode = domRegionNode.item(j);
        DOM_Element vol = (DOM_Element&) volNode;

        std::string volname = dom2Std (vol.getAttribute ("name"));
        volvect.push_back(volname);        
        }
      
      RegionCuts rcut(regname,
                      volvect,
                      regcut[prcut].gammacut,
                      regcut[prcut].electroncut,
                      regcut[prcut].positroncut,
                      regcut[prcut].protoncut,
                      regcut[prcut].aprotoncut,
                      regcut[prcut].neutroncut,
                      regcut[prcut].aneutroncut);
      m_regionsDefs.push_back(rcut);
    }
  }
}

// -----------------------------------------------------------------------
//  hasSimAttribute (ILVolume*)
// -----------------------------------------------------------------------
const bool SimulationSvc::hasSimAttribute (const ILVolume* vol) const {
  if (0 != vol) {
    // try first to find an attribute associated directly to the logical volume
    std::string bnn = vol->name();
    Dict::const_iterator it = m_logvol2Sim.find (bnn);
    if (it != m_logvol2Sim.end()) {
      return true;
    }    
  }
  return false;
}

// -----------------------------------------------------------------------
//  hasSimAttribute (std::string)
// -----------------------------------------------------------------------
const bool SimulationSvc::hasSimAttribute (const std::string volname) const {

  // try first to find an attribute associated directly to the logical volume
  Dict::const_iterator it = m_logvol2Sim.find (volname);
  if (it != m_logvol2Sim.end()) {
    return true;
  }    
  return false;
}

// -----------------------------------------------------------------------
//  simAttribute (ILVolume*)
// -----------------------------------------------------------------------
const SimulationSvc::PartAttr*
SimulationSvc::simAttribute (const ILVolume* vol) const {
  PartAttr* part;
  
  if (0 != vol) {
    // try first to find an attribute associated directly to the logical volume
    std::string bnn = vol->name();
    Dict::const_iterator it = m_logvol2Sim.find (bnn);
    if (it != m_logvol2Sim.end()) 
      {
        part=it->second;
      } 
    else 
      {
        MsgStream log(msgSvc(), name());
  log << MSG::WARNING << "No SimAttribute for " 
      << vol->name() << endreq;
      } 
  }
  return part;
}

// -----------------------------------------------------------------------
//  simAttribute (std::string)
// -----------------------------------------------------------------------
const SimulationSvc::PartAttr*
SimulationSvc::simAttribute (std::string volname) const {
  PartAttr* part;
  
  // try first to find an attribute associated directly to the logical volume
  Dict::const_iterator it = m_logvol2Sim.find (volname);
  if (it != m_logvol2Sim.end()) 
    {
      part=it->second;
    } 
  else 
    {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "No SimAttribute for " 
          << volname << endreq;
    }
  return part;
}

//------------------------------------------------------------------------
// regionsDefs()
//------------------------------------------------------------------------

const std::vector<RegionCuts>* SimulationSvc::regionsDefs () const
{
  return &m_regionsDefs;
}


// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode
SimulationSvc::queryInterface(const IID& riid, void** ppvInterface) {
  if (IID_ISimulationSvc.versionMatch(riid))  {
    *ppvInterface = (ISimulationSvc*)this;
  } else {
    // Interface is not directly availible: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
