// $Id: AlignmentElement.cpp,v 1.5 2008-01-07 11:01:16 janos Exp $
// Include files

// from STD
#include <iomanip>
#include <algorithm>
#include <functional>

// Gaudi math
#include "GaudiKernel/Vector3DTypes.h"

// from DetDesc
#include "DetDesc/ParamException.h"
#include "DetDesc/GeometryInfoException.h"

// from BOOST
#include "boost/regex.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "AlignmentElement.h"

AlignmentElement::AlignmentElement(const DetectorElement* element, const unsigned int index, 
                                   const std::vector<bool>& dofs)
  : m_elements(1u, element),
    m_index(index),
    m_dofs(dofs) {

  validDetectorElement(m_elements.at(0u));
  // std::for_each(m_elements.begin(), m_elements.end(),
  // boost::lambda::bind(&AlignmentElement::validDetectorElement, this, boost::lambda::_1));

  setPivotPoint();
}

AlignmentElement::AlignmentElement(const std::vector<const DetectorElement*>& elements, const unsigned int index, 
                                   const std::vector<bool>& dofs)
  : m_elements(elements),
    m_index(index),
    m_dofs(dofs) {

  std::for_each(m_elements.begin(), m_elements.end(),
                boost::lambda::bind(&AlignmentElement::validDetectorElement, this, boost::lambda::_1));

  setPivotPoint();
}

void AlignmentElement::validDetectorElement(const DetectorElement* element) const {
  /// Check that it doesn't point to null
  if (!element) throw GeometryInfoException("DetectorElement* points to NULL!");
  /// Check that it has alignment conditions
  if (!(element->geometry()->alignmentCondition())) throw GeometryInfoException("AlignmentCondition* points to NULL!");
}

void AlignmentElement::setPivotPoint() {
  /// Set pivot point
  /// Here we take the average of the global centres of the elements as the pivot point
  // const Gaudi::XYZPoint lhcbOrigin(0.0, 0.0, 0.0);
  Gaudi::XYZVector averageR(0.0, 0.0, 0.0);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    averageR += ((*i)->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0))) - Gaudi::XYZPoint(0.0, 0.0, 0.0);
  }
  averageR /= double(m_elements.size());
  m_pivot = Gaudi::XYZPoint(0.0, 0.0, 0.0) + averageR;
}

const std::string AlignmentElement::name() const {
  std::string begin = (m_elements.size() > 1u ? "Group = { " : "");

  std::string middle;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    std::string nameElem = boost::algorithm::erase_all_regex_copy((*i)->name(), boost::regex("/dd/Structure/LHCb/.*Region/"))
      + (i != (iEnd - 1u)?", \n":"\n");
    middle += nameElem;
  }

  std::string end = (m_elements.size() > 1u ? "}" : "");

  return begin+middle+end;
}

const std::vector<double> AlignmentElement::deltaRotations() const {  
  std::vector<double> averageDeltaRotations(0u,3);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    std::vector<double> elemDeltaRotations = (*i)->geometry()->alignmentCondition()->paramVect<double>("dRotXYZ");
    std::transform(elemDeltaRotations.begin(), elemDeltaRotations.end(), averageDeltaRotations.begin(),
                   averageDeltaRotations.begin(), std::plus<double>());
  }
  std::transform(averageDeltaRotations.begin(), averageDeltaRotations.end(),
                 averageDeltaRotations.begin(), boost::lambda::bind(&AlignmentElement::average, this, boost::lambda::_1));
  
  return averageDeltaRotations;
}


const std::vector<double> AlignmentElement::deltaTranslations() const {
  std::vector<double> averageDeltaTranslations(0u,3);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    std::vector<double> elemDeltaTranslations = (*i)->geometry()->alignmentCondition()->paramVect<double>("dPosXYZ");
    std::transform(elemDeltaTranslations.begin(), elemDeltaTranslations.end(), averageDeltaTranslations.begin(),
                   averageDeltaTranslations.begin(), std::plus<double>());
  }
  std::transform(averageDeltaTranslations.begin(), averageDeltaTranslations.end(),
                 averageDeltaTranslations.begin(), boost::lambda::bind(&AlignmentElement::average, this, boost::lambda::_1));
  
  return averageDeltaTranslations;
}

unsigned int AlignmentElement::nDOFs() const {
  unsigned int nDOF  = 0u;
  for (std::vector<bool>::const_iterator i = m_dofs.begin(), iEnd = m_dofs.end(); i != iEnd; ++i) if ((*i)) ++nDOF;
  return nDOF;
}
  
StatusCode AlignmentElement::setLocalDeltaParams(const std::vector<double>& localDeltaT,
                                                 const std::vector<double>& localDeltaR) const {
  StatusCode sc;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    sc = const_cast<IGeometryInfo*>((*i)->geometry())->ownToOffNominalParams(localDeltaT, localDeltaR, this->pivot());
    if (sc.isFailure()) {
      std::cout << "Failed to update local delta parameters of detector element " + (*i)->name() <<  std::endl;
      break; ///< Break loop if sc is failure
    }
  }
  return sc;
}

bool AlignmentElement::operator==(const DetectorElement* rhs) const {
  ElemIter match = std::find(m_elements.begin(), m_elements.end(), rhs);
  return (match != m_elements.end() ? true : false);
}

StatusCode AlignmentElement::setLocalDeltaMatrix(const std::vector<double>& globalDeltaT,
                                                 const std::vector<double>& globalDeltaR) const {
  /// Construct global delta matrix from deltas
  const Gaudi::Transform3D globalDeltaMatrix = DetDesc::localToGlobalTransformation(globalDeltaT,
                                                                                    globalDeltaR,
                                                                                    this->pivot());
  
  StatusCode sc;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    /// Transform global delta matrix to new local delta matrix (takes current local delta matrix into account!!)
    const Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix((*i), globalDeltaMatrix);
    /// Update position of detector element with new local delta matrix
    sc = const_cast<IGeometryInfo*>((*i)->geometry())->ownToOffNominalMatrix(localDeltaMatrix);
    if (sc.isFailure()) {
      std::cout << "Failed to update local delta matrix of detector element " + (*i)->name() <<  std::endl;
      break; ///< Break loop if sc is failure
    }
  }

  return sc;
}

// inline const Gaudi::Transform3D AlignmentElement::localDeltaMatrix() const {
//   return m_element->geometry()->ownToOffNominalMatrix();
// }

const Gaudi::XYZPoint AlignmentElement::localCentre() const {

  Gaudi::XYZVector avgLocalCentre(0.0, 0.0, 0.0);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    avgLocalCentre += ((*i)->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0))) - Gaudi::XYZPoint(0.0, 0.0, 0.0);
  }

  avgLocalCentre /= double(m_elements.size());

  return Gaudi::XYZPoint(0.0, 0.0, 0.0) + avgLocalCentre;
}

std::ostream& AlignmentElement::fillStream(std::ostream& lhs) const {
  const std::string s = name();
  const std::vector<double>& t = deltaTranslations();
  const std::vector<double>& r = deltaRotations();
  lhs << std::endl;
  lhs << std::left << std::setw(s.size()+30u) << std::setfill('*') << "" << std::endl;
  lhs << "* Element  : " << s << "\n"
      << "* Index    : " << index() << "\n"
      << "* dPosXYZ  : " << Gaudi::XYZPoint(t[0], t[1], t[2]) << "\n"
      << "* dRotXYZ  : " << Gaudi::XYZPoint(r[0], r[1], r[2]) << "\n"
      << "* pivotXYZ : " << pivotXYZPoint() << std::endl;
  lhs << std::left << std::setw(s.size()+30u) << std::setfill('*') << "" << std::endl;

  return lhs;
}
