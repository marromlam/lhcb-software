
#include "CLHEP/Geometry/Point3D.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

//
#include "GaudiKernel/MsgStream.h"

// GiGa 
#include "GiGa/GiGaStackActionFactory.h"


/// local
#include "GiGaStackActionEmpty.h"




/////////////////////////////////////////////////////////////////////////////////////////////////
static const GiGaStackActionFactory<GiGaStackActionEmpty>                      s_GiGaStackActionEmptyFactory;
const       IGiGaStackActionFactory&             GiGaStackActionEmptyFactory = s_GiGaStackActionEmptyFactory;
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStackActionEmpty::GiGaStackActionEmpty( const std::string& Name , ISvcLocator* Loc ) 
  : GiGaStackAction            ( Name , Loc ) 
{};
/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStackActionEmpty::~GiGaStackActionEmpty(){};
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionEmpty::initialize () { return GiGaStackAction::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionEmpty::finalize   () { return GiGaStackAction::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
