// $Id: RichDAQDefinitions.h,v 1.2 2003-11-10 14:59:59 jonrob Exp $
#ifndef RICHDAQ_RICHDAQDEFINITIONS_H 
#define RICHDAQ_RICHDAQDEFINITIONS_H 1

// from STL
#include <vector>

// Event model
#include "Event/DAQTypes.h"
#include "Event/MCRichDigit.h"
#include "Event/RichDigit.h"
#include "Event/HltEvent.h"

// Include files
#include "RichKernel/RichSmartID.h"

namespace Rich {

  // Data defintions
  typedef hlt_int        LongType;
  typedef unsigned int   ShortType;

  // Container for PD data
  typedef std::map< RichSmartID, MCRichDigitVector > PDMap;

  // Local defintion of an HltBank constructed from 32bit ints
  typedef std::vector<Rich::LongType>                HLTBank;

  // Container of Rich HltBanks
  typedef std::vector<HltBank>                       HLTBanks;

  // Container of RichSmartIDs
  typedef std::vector<RichSmartID>                   SmartIDs;

}

#endif // RICHDAQ_RICHDAQDEFINITIONS_H
