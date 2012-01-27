#!/usr/bin/env python
# =============================================================================
# $Id: StdTightANNElectrons.py,v 1.5 2010-06-01 09:41:12 odescham Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightANNElectrons.py
#  configuration file for 'Standard Loose Electrons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Tight ANN Electrons'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdTightANNElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdTightANNElectrons',
                                    Particle = 'electron'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter , 'Electron' )
#fltr.Selection = [ "RequiresDet='CALO' ProbNNe>'0.2'" ]
#fltr.Selection = [ "ProbNNe>'0.2'" ]
fltr.Selection = [ "RequiresDet='RICHORCALO' ProbNNe>'0.35'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightANNElectrons = algorithm 

## ============================================================================
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
