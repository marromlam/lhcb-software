#ifndef BTAGGINGTOOL_IBTAGGINGTOOL_H
#define BTAGGINGTOOL_IBTAGGINGTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/FlavourTag.h"
#include "Event/RecVertex.h"

/** @class IBTaggingTool IBTaggingTool.h
 *
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2004-12-14
 */

class GAUDI_API IBTaggingTool : virtual public IAlgTool 
{

public:

  DeclareInterfaceID(IBTaggingTool, 2, 0);

 public:

  virtual StatusCode tag( LHCb::FlavourTag&, 
                          const LHCb::Particle* ) = 0;
  virtual StatusCode tag( LHCb::FlavourTag&, 
                          const LHCb::Particle*, 
                          const LHCb::RecVertex* ) = 0;
  virtual StatusCode tag( LHCb::FlavourTag&, 
                          const LHCb::Particle*, 
                          const LHCb::RecVertex*,
                          LHCb::Particle::ConstVector& ) = 0;
};

#endif // BTAGGINGTOOL_IBTAGGINGTOOL_H
