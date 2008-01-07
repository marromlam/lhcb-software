// $Id: GetElementsToBeAligned.cpp,v 1.7 2008-01-07 11:01:16 janos Exp $
// Include files

//from STL
#include <iomanip>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from Boost
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"

// // local
#include "GetElementsToBeAligned.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetElementsToBeAligned
//
// 2007-10-08 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GetElementsToBeAligned );

GetElementsToBeAligned::GetElementsToBeAligned( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_groupElems(false),
    m_elemsToBeAligned(),
    m_depth(0),
    m_index(0),
    m_elements(),
    m_rangeElements(),
    m_constraints(0),
    m_findElement()
{
  declareInterface<IGetElementsToBeAligned>(this);
  declareProperty("Elements"     , m_elemsToBeAligned);
  declareProperty("GroupElements", m_groupElems      );
  declareProperty("Constraints"  , m_constraints     );
}

GetElementsToBeAligned::~GetElementsToBeAligned() {}

StatusCode GetElementsToBeAligned::initialize() {

  StatusCode sc =  GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize tool!", sc);

  if (m_elemsToBeAligned.empty()) return Error("Please specify which elements to align!", sc);

  /// Get pointer to detector svc
  IDataProviderSvc* detectorSvc = svc<IDataProviderSvc>("DetectorDataSvc", true);

  /// Retrieve LHCb detector in Transient store
  const std::string lhcbDetectorPath = "/dd/Structure/LHCb";
  DataObject* lhcbObj = 0;
  sc = detectorSvc->retrieveObject(lhcbDetectorPath, lhcbObj);
  if (sc.isFailure()) {
    error() << "==> Couldn't retrieve LHCb Detector from transient store!" << endmsg;
    detectorSvc->release();
    return sc;
  }

  IDetectorElement* lhcb = dynamic_cast<IDetectorElement*>(lhcbObj);
  /// This shouldn't go wrong, but you never know
  if (!lhcb) {
    error() << "==> Object is not a detector element!" << endmsg;
    detectorSvc->release();
    return StatusCode::FAILURE;
  }

  info() << "===================== GetElementsToAlign =====================" << endmsg;
  info() << "   Using the following regular expressions: " << endmsg;
 
  std::vector<std::string> groupsOfElements = m_elemsToBeAligned.begin()->second;
  for (std::vector<std::string>::const_iterator i = groupsOfElements.begin(), iEnd = groupsOfElements.end(); i != iEnd; ++i) {
    /// Forward slash is the path separator
    Separator sep("/");
    /// Tokens of regular expressions
    Tokenizer regexs((*i), sep);

    /// Print the list of regular expressions that are going to be used.
    info() << "       ";
    for (Tokenizer::iterator j = regexs.begin(), jEnd = regexs.end(); j != jEnd; ++j) info() << "\"" << (*j) << "\"" << " ";
    info() << endmsg;
    
    /// Create list of regular expressions
    for (Tokenizer::iterator j = regexs.begin(), jEnd = regexs.end(); j != jEnd; ++j) {
      boost::regex ex;
      /// Check if token is a valid regular expression, else catch exception and return statuscode failure.
      /// Better to stop the program and let the user fix the expression than trying to predict what he/she wants.
      try {
        ex.assign((*j), boost::regex_constants::icase);
      } catch (boost::bad_expression& exprs) {
        error() << "==> Error: " << (*j) << " is not a valid regular expression: " << exprs.what() << endmsg;
        return StatusCode::FAILURE;
      }
      /// If valid add expression to list of expressions.
      m_regexs.push_back(ex);
    }

    /// Depth is equal to the number regular expressions in the regex list.
    m_depth = m_regexs.size();
    
    /// Traverse LHCb detector in transient store and get alignable elements
    getElements(lhcb);
    
    /// loop over elements and create AlignmentElements
    if (m_groupElems) m_alignElements.push_back(AlignmentElement(m_elements, m_index++));
    else std::transform(m_elements.begin(), m_elements.end(), std::back_inserter(m_alignElements),
                        boost::lambda::bind(boost::lambda::constructor<AlignmentElement>(), boost::lambda::_1, boost::lambda::var(m_index)++));
    
    m_regexs.clear();
    m_depth = 0u;
    m_elements.clear();
  
  }
  
  m_rangeElements = std::make_pair(m_alignElements.begin(), m_alignElements.end());
  m_findElement   = FindAlignmentElement(Alignment::FindElement(m_elemsToBeAligned.begin()->first), m_rangeElements);


  /// Print list of detector elements that are going to be aligned
  info() << "   Going to align " << std::distance(m_rangeElements.first, m_rangeElements.second) << " detector elements:" << endmsg;
  typedef std::vector<AlignmentElement>::const_iterator ElemIter;
  for (ElemIter i = m_rangeElements.first, iEnd = m_rangeElements.second; i != iEnd; ++i) {
    info() <<  "        " << "Element " << i->name() << " with index " << i->index() << " and pivot " << i->pivotXYZPoint() << endmsg;
  }
  info() << "   With " << m_constraints.size() << " constraints:" << endmsg;
  unsigned nC = 0u;
  bool clear = false;
  for (IGetElementsToBeAligned::Constraints::const_iterator i = m_constraints.begin(), iEnd = m_constraints.end(); i != iEnd;
       ++i, ++nC) {
    if (i->empty() || i->size()-1u !=  std::distance(m_rangeElements.first, m_rangeElements.second)*6u) {
      warning() << "Number of elements do not much total number of degrees of freedom!" << endmsg;
      warning() << "Setting constraints to ZERO!" << endmsg;
      clear = true;
      break;
    }
    info() << "       " << nC << ": f = [ ";
    for (std::vector<double>::const_iterator j = i->begin(), jEnd = i->end()-1u; j != jEnd; ++j) {
      info()  << (*j) << " ";
    }

    info() << " ] ";
    info() << " and f0 = " << i->back() << endmsg;
  }
  if (clear) m_constraints.clear();

  info() << "==============================================================" << endmsg;

  return sc;
}

void GetElementsToBeAligned::getElements(const IDetectorElement* parent) {
  /// loop over children
  for (IDetIter iC = parent->childBegin(), iCend = parent->childEnd(); iC != iCend; ++iC) {
    /// Get path of child
    const std::string childPath = (*iC)->name();
    if (msgLevel(MSG::DEBUG)) debug() << "==> Element  = " << childPath << endmsg;

    /// forward slash is path seperator
    Separator sep("/");
    /// Tokenise path
    Tokenizer tokenizer(childPath, sep);

    std::list<std::string> paths;
    /// Copy tokens to a temporary list of sub paths. This allows us to determine how deep we are.
    std::copy(tokenizer.begin(), tokenizer.end(), std::back_inserter(paths));

    const size_t currentDepth = paths.size();

    bool match = true;         ///< Can we match a sub path to a regular expression?

    /// Begin iterator of regular expressions list
    std::list<boost::regex>::const_iterator iR = m_regexs.begin();
    /// End iterator of regular expressions list
    std::list<boost::regex>::const_iterator iRend = m_regexs.end();
    /// Loop over list of sub paths and try to match them
    for (std::list<std::string>::const_iterator i = paths.begin(), iEnd = paths.end();
	 i != iEnd && iR != iRend; ++i) {
      match = boost::regex_match((*i), (*iR++));
      if (!match) break; /// If it doesn't match break loop. No need to try and match the rest
    }

    /// OK we found a detector element
    if (match) {
      if (currentDepth == m_depth) {
        m_elements.push_back(dynamic_cast<DetectorElement*>(*iC));
      }
      /// Call thyself. Get children recursively.
      /// No need to go any deeper.
      if (paths.size() < m_depth) getElements((*iC));
    }
  }
}

const IGetElementsToBeAligned::Range& GetElementsToBeAligned::rangeElements() const {
  return m_rangeElements;
}

const IGetElementsToBeAligned::Constraints& GetElementsToBeAligned::constraints() const {
  return m_constraints;
}

const AlignmentElement* GetElementsToBeAligned::findElement(LHCb::LHCbID anLHCbID) const {
  return m_findElement(anLHCbID);
}
