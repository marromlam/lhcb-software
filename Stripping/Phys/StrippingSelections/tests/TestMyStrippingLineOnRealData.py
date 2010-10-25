# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf


# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")






#---->
# Import your stripping lines
from StrippingSelections.StrippingBd2KstarMuMuTriggered import StrippingBd2KstarMuMuConf
from StrippingSelections.StrippingBd2KstarMuMuTriggered import defaultConfig as Bd2KstarMuMuConfig
from StrippingSelections.StrippingBd2KstarMuMuTriggered import defaultLines as Bd2KstarMuMuLines
Bd2KstarMuMuConf = StrippingBd2KstarMuMuConf( config= Bd2KstarMuMuConfig, activeLines=Bd2KstarMuMuLines )
stream.appendLines(Bd2KstarMuMuConf.lines())

#
from StrippingSelections.StrippingBs2PhiMuMu import StrippingBs2PhiMuMuConf
stream.appendLines([StrippingBs2PhiMuMuConf().Bs2PhiMuMuLine()])

#
from StrippingSelections.StrippingBs2MuMuPhi import StrippingBs2MuMuPhiConf
from StrippingSelections.StrippingBs2MuMuPhi import defaultConfig as Bs2MuMuPhiConfig
Bs2MuMuPhiConf = StrippingBs2MuMuPhiConf(config = Bs2MuMuPhiConfig)
stream.appendLines(Bs2MuMuPhiConf.lines())

# Import your stripping lines
from StrippingSelections import StrippingDstarVeryLooseWithD02Kpi 
confDstarVeryLooseWithD02Kpi = StrippingDstarVeryLooseWithD02Kpi.DstarVeryLooseWithD02KpiAllLinesConf(StrippingDstarVeryLooseWithD02Kpi.confdict)
stream.appendLines( confDstarVeryLooseWithD02Kpi.Lines )

from StrippingSelections import StrippingBd2DstarMuNu
confBd2DstarMuNu = StrippingBd2DstarMuNu.Bd2DstarMuNuAllLinesConf(StrippingBd2DstarMuNu.confdict)
stream.appendLines( confBd2DstarMuNu.Lines )


#<----







from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Mimic the new default tracking cuts
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts = { "Chi2Cut" : [0,5] }

# Configure the ETC writing step
from Configurables import EventTuple, TupleToolSelResults
from Configurables import TupleToolStripping

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolStripping"  ]
tag.addTool( TupleToolStripping )
tag.TupleToolStripping.StrippigReportsLocations = "/Event/Strip/Phys/DecReports"

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

seq = GaudiSequencer("TagSeq")
seq.Members = [tag]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())




#---->
# vanyas suggestion of correlation matrix here:
from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())
#<----





from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 200000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().MoniSequence += [ seq ]            # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79646_RealData+Reco06-Stripping10_90000000_SDST.py")
#more statistics (need to add += in the data .py file):
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79647_RealData+Reco06-Stripping10_90000000_SDST.py")


#DaVinci().Simulation = True

