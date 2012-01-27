#!/usr/bin/env python
# =============================================================================
# $Id: StdTightANNMuons.py,v 1.7 2009-08-04 09:05:31 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightANNMuons.py
#  configuration file for 'Standard Tight Muons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Tight Muons'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $"
# =============================================================================
__all__ = (
    'StdTightANNMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleMUONFilter

from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdTightANNMuons' ,
                                    Particle = 'muon'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleMUONFilter, 'Muon' )
#fltr.Selection = [ "RequiresDet='MUON' IsMuon=True ProbNNmu>'0.1'" ]
fltr.Selection = [ "ProbNNmu>'0.1'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightANNMuons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
