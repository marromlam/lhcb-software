// $Id: StateThinMSCorrectionTool.h,v 1.3 2008-03-31 07:13:28 mneedham Exp $
#ifndef TRACKTOOLS_STATETHINMSCORRECTIONTOOL_H 
#define TRACKTOOLS_STATETHINMSCORRECTIONTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"

// from TrackEvent
#include "Event/State.h"

/** @class StateThinMSCorrectionTool StateThinMSCorrectionTool.h
 *  
 *  This state correction tool applies a multiple scattering correction
 *  in the approximation of a thin scatter
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-08-21
 *  (Original code taken from the master extrapolator)
 */
class StateThinMSCorrectionTool : public GaudiTool,
                                  virtual public IStateCorrectionTool {
public:
  /// Correct a State for multiple scattering in the approximation of a thin scatter
  void correctState( LHCb::State& state,
                     const Material* material,
                     double wallThickness = 0,
                     bool upstream = true,  
                     LHCb::ParticleID pid = LHCb::ParticleID(211) ) const ;
  
  /// Standard constructor
  StateThinMSCorrectionTool( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);
  
  virtual ~StateThinMSCorrectionTool( ); ///< Destructor
  
  StatusCode initialize();
  
protected:
  
private:
  // Job options
  double m_msff2;    ///< fudge factor for multiple scattering errors

};
#endif // TRACKTOOLS_STATETHINMSCORRECTIONTOOL_H
