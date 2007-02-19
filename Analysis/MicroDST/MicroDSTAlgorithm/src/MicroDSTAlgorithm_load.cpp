// $Id: MicroDSTAlgorithm_load.cpp,v 1.2 2007-02-19 09:22:04 ukerzel Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(MicroDST) {
  DECLARE_ALGORITHM(TestDST)
  DECLARE_ALGORITHM(CreateMicroDSTAlg)  
  DECLARE_ALGORITHM(CreateMicroDSTMCAlg)  
}
