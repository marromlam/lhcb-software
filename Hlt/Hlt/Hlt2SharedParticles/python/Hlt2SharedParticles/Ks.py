### @file
#
# Standard Ks
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.GoodParticles import GoodPions
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions
from Configurables import CombineParticles
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'KsLL', 'KsDD' )

Hlt2SharedKsLL = Hlt2Member( CombineParticles, "KsLL"
                           , DecayDescriptor = "KS0 -> pi+ pi-" 
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" } 
                           , CombinationCut = "(ADAMASS('KS0')<50*MeV)"
                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2/VDOF)<30)"
                           , InputLocations = [ GoodPions ]
                           )

KsLL = bindMembers( "SharedKsLL", [ GoodPions, Hlt2SharedKsLL ] )

# Now the downstream K shorts, requires fitted tracks!
Hlt2SharedKsDD = Hlt2Member( CombineParticles, "KsDD"
                           , DecayDescriptor = "KS0 -> pi+ pi-"
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" }
                           , CombinationCut = "(ADAMASS('KS0')<80*MeV)"
                           , MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2/VDOF)<30)"
                           , InputLocations = [ BiKalmanFittedDownPions ]
                           )

KsDD = bindMembers( "SharedKsDD", [ BiKalmanFittedDownPions , Hlt2SharedKsDD ] )
