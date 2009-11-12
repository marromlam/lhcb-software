# Test, will read teh FSR from a DST file
# These options are mostly copied from $IOEXAMPLEROOT/tests/options/ReadDst.opts
from Gaudi.Configuration import *
from Configurables import EventSelector, LHCbApp
from LumiAlgs.Configuration import *

# Set up POOL persistency
importOptions( "$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts" )
#Select DST file
EventSelector().Input   = [
  "DATAFILE='PFN:56721-100ev.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]

# Run LumiTest
#importOptions("$LUMIALGSROOT/job/LumiAlgsConfTest.py")

#select correct data format
LumiAlgsConf().InputType='DST'
LumiAlgsConf().DataType='2009'
#print verbose output
LumiAlgsConf().OutputLevel=1

lSeq=GaudiSequencer("LumiSequence")

LumiAlgsConf().LumiSequencer=lSeq

lSeq.OutputLevel=1

LHCbApp().EvtMax = 100

ApplicationMgr().TopAlg+=[lSeq]
ApplicationMgr().OutputLevel=1

