# Example 2011 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION11-Beam3500GeV-VeloClosed-MagDown.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Escher

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
Escher().DataType = "2011"
Escher().InputType  = "MDF"
Escher().DDDBtag = 'dddb-20130503'
Escher().CondDBtag = 'cond-20130522'

# Events from run 87968 from fill 1647, on 24th March 2011
from GaudiConf import IOHelper
IOHelper('MDF').inputFiles([
    "mdf:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/LHCb/Collision11/RAW/00020657/0000/00020657_00000391_1.raw"
    ], clear=True)
