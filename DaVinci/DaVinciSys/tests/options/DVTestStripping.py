#
#   Stripping selections test job
#
#   @author A. Poluektov
#   @date 2009-05-14
#

from Gaudi.Configuration import *
from Configurables import GaudiSequencer

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

from StrippingConf.Configuration import StrippingConf
from StrippingSelections import StreamBmuon, StreamHadron, StreamJpsi, StreamDstar, StreamLambda, StreamBelectron

sc = StrippingConf()
sc.appendStream( StreamBmuon.stream )
sc.appendStream( StreamHadron.stream )
sc.appendStream( StreamJpsi.stream )
sc.appendStream( StreamDstar.stream )
sc.appendStream( StreamLambda.stream )
sc.appendStream( StreamBelectron.stream )

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults)

tag.TupleToolSelResults.Selections = sc.selections()  # Add the list of stripping selections to TagCreator

#
# Configure DaVinci
#

from Configurables import DaVinci

DaVinci().appendToMainSequence( [ sc.sequence() ] )   # Append the stripping selection sequence to DaVinci
DaVinci().appendToMainSequence( [ tag ] )             # Append the TagCreator to DaVinci
DaVinci().EvtMax = 100                         # Number of events
DaVinci().DataType = "MC09"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It is MC
DaVinci().ETCFile = "etc.root"

importOptions("$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.py")
