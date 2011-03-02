#
#   Radiative stripping selection
#
#   @author Greig Cowan and Tom Blake 
#   @date 2011-03-02
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingSettings.Utils import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream('Radiative')

# RD WG 
configs = [
	'B2XGamma'
	]

for config in configs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )


