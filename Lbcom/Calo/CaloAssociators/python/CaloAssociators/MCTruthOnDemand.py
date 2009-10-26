#!/usr/bin/env python
# =============================================================================
## @file
#  Configuration file to run Calo-MC-associations on-demand
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-08-08
# =============================================================================
"""

Configuration file to run Calorimeter Monte Carlo Truth Associations on-demand

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
# nothing to be imported 
__all__     = (
    ) 
# =============================================================================
from Gaudi.Configuration   import *

from Configurables         import ( CaloDigit2MCLinks2Table ,
                                    CaloClusterMCTruth      )

from Configurables         import ( CaloZSupAlg       ,
                                    CaloDigitsFromRaw ) 

from CaloKernel.ConfUtils  import ( onDemand       ,
                                    printOnDemand  ,
                                    getAlgo        ) 

# =============================================================================
linker2table = getAlgo ( GaudiSequencer                , 
                         'CaloDigit2MC'                ,
                         'Offline'                     ,
                         'Relations/Raw/Calo/Digits'   ,
                         True                          )

linker2table.Members += [CaloZSupAlg       ( "EcalZSup"        ) ,
                         CaloZSupAlg       ( "HcalZSup"        ) ,
                         CaloDigitsFromRaw ( "PrsFromRaw"      ) ,
                         CaloDigitsFromRaw ( "SpdFromRaw"      ) ,
                         CaloDigit2MCLinks2Table( "CaloDigit2MCLinks" )
                         ]


for context in ( 'Offline' ,
                 'HLT'     ) : 
    getAlgo ( CaloClusterMCTruth            ,   
              'CaloClusterMCTruth'          ,
              context                       , 
              'Relations/Rec/Calo/Clusters' ,
              True                          )
    

# =============================================================================
if '__main__' == __name__ :
    
    print __doc__
    print printOnDemand() 
    
# =============================================================================
# The END
# =============================================================================
