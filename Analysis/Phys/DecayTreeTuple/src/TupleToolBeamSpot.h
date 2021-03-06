// Include files

// from LHCb
#include "Kernel/IEventTupleTool.h"
#include "DecayTreeTupleBase/TupleToolBase.h"

#include "LoKi/BeamSpot.h"

/** @class TupleToolBeamSpot DecayTreeTuple/TupleToolBeamSpot.cpp
*
* Add beamspot position (from the Velo resolver position)
*
* Tuple columns:
*  - BeamX
*  - BeamY
*
* @see LoKi::BeamSpot
*
* @author Pieter David <pieter.david@nikhef.nl>
* @date   2015-07-10
*/
class TupleToolBeamSpot : public TupleToolBase, virtual public IEventTupleTool
{
public:
  /// Standard constructor
  TupleToolBeamSpot ( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~TupleToolBeamSpot(); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Interface methods
  virtual StatusCode fill( Tuples::Tuple& tuple );

private:
  double m_bound;
  std::unique_ptr<LoKi::BeamSpot> m_beamspot;
};
