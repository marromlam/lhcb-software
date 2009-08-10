########################################################################
#
# $Id: DVTestHlt12-MDF.py,v 1.5 2009-08-10 15:08:02 pkoppenb Exp $
#
# Test for Hlt1&2. Test for MDF files.
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().HltThresholdSettings = 'Miriam_20090430'
DaVinci().EvtMax = 10000                       # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "2009"                    # Default is "MC09", pretend it's real data
# DaVinci().Simulation   = False 
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().InputType = "MDF"
DaVinci().HltThresholdSettings = 'Effective_Nominal'
EventSelector().Input   = [
    "   DATA='PFN:castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_00000002_1.mdf' SVC='LHCb::MDFSelector'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []

appendPostConfigAction(noInit)
