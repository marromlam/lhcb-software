// $Id: GetMCRichOpticalPhotonsAlg.h,v 1.4 2009-03-26 21:49:47 robbep Exp $
#ifndef SENSDET_GETMCRICHOPTICALPHOTONSALG_H
#define SENSDET_GETMCRICHOPTICALPHOTONSALG_H 1

// base class
#include "GetMCRichInfoBase.h"

// rich kernel
#include "RichKernel/RichMap.h"

/** @class GetMCRichOpticalPhotonsAlg GetMCRichOpticalPhotonsAlg.h
 *
 *  Algorithm to create MCRichOpticalPhoton objects from Gauss G4 information
 *
 *  @author Sajan EASO
 *  @date   2005-12-06
 */

class GetMCRichOpticalPhotonsAlg : public GetMCRichInfoBase
{

public:

  /// Standard constructor
  GetMCRichOpticalPhotonsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichOpticalPhotonsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;

  /// map for photon counting
  typedef Rich::Map< const Rich::RadiatorType, unsigned long int > RadMap;

  /// count overall number of photons in each RICH radiator medium
  RadMap m_hitTally;

  /// Location of MC Rich Hits
  std::string m_mcRichHits ;
};

#endif // SENSDET_GETMCRICHOPTICALPHOTONSALG_H
