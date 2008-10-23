### @file 
 #
 # Hlt exclusive selections options
 #
 # @author P. Koppenburg
 # @date 2006-02-01
###
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
# ---------------------------------------------------------------------
# Common stuff. Probably already included in Hlt1.
# ---------------------------------------------------------------------
importOptions( "$HLTCONFROOT/options/HltMainSequence.opts" )
# ---------------------------------------------------------------------
#  Phys initialisation
# ---------------------------------------------------------------------
importOptions( "$HLTCONFROOT/options/Hlt2Init.opts" )
# ---------------------------------------------------------------------
#  HLT exclusive Selections
# ---------------------------------------------------------------------
Hlt2 = GaudiSequencer("Hlt2")
GaudiSequencer("Hlt").Members += [ Hlt2 ]
Hlt2.Context = "HLT" 
#  just in case
from Configurables import HltSummaryWriter
Hlt2.Members += [ HltSummaryWriter() ]
# ---------------------------------------------------------------------
#  Make sure Hlt1 is passed
# ---------------------------------------------------------------------
Hlt2.Members += [ GaudiSequencer("Hlt2CheckHlt1Passed") ]
GaudiSequencer("Hlt2CheckHlt1Passed").Members = [ GaudiSequencer("PassedAlleys") ]
# ---------------------------------------------------------------------
#  Full reconstruction of all tracks 
# ---------------------------------------------------------------------
Hlt2.Members += [ GaudiSequencer("HltRecoSequence") ]
# ---------------------------------------------------------------------
#  ProtoParticles and Particles
# ---------------------------------------------------------------------
importOptions( "$HLTCONFROOT/options/Hlt2Particles.opts" )
# ---------------------------------------------------------------------
#  Empty Sequencer for MC truth filtering --- filled in HltSelChecker
# ---------------------------------------------------------------------
Hlt2.Members += [ GaudiSequencer("SeqHlt2TruthFilter") ] 
importOptions( "$HLTCONFROOT/options/HltTrackAssociator.opts" )
# ---------------------------------------------------------------------
# Initialise Hlt2Shared particles
# ---------------------------------------------------------------------
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Shared.py" )
# ---------------------------------------------------------------------
#  Final selections
# ---------------------------------------------------------------------
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Selections.py" )
# --------------------------------------------------------------------
#  Decision
# --------------------------------------------------------------------
Hlt2.Members += [ "HltSelectionsDecision/Hlt2Decision" ] 

