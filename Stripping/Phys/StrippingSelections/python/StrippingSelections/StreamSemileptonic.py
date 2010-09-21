#
#   Stripping selections 
#
#   B (and charm) decays with one muon required, exclusive and
#   semi-inclusive selections.
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingBd2DstarMuNu
from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
#from StrippingSelections import StrippingBsDsMuNu
#superceded by B0q2DplusMuX
from StrippingSelections import Strippingb2DMuX


from StrippingSelections.StrippingB2XuMuNu import StrippingB2XuMuNuConf
from StrippingSelections.StrippingB2DMuNuX import StrippingB2DMuNuXConf

stream = StrippingStream("Semileptonic")
stream.appendLines( [ 
		        StrippingBd2DstarMuNu.line, 
		        StrippingBd2DstarMuNuLooseConf().line(), 
		        #StrippingBsDsMuNu.line, 
                        StrippingB2DMuNuXConf().b2D0MuXLine(),
                        StrippingB2DMuNuXConf().b2DpMuXLine(),
                        StrippingB2DMuNuXConf().b2DsMuXLine(),
                	StrippingB2DMuNuXConf().b2LcMuXLine()                                                  
                        # Lines from P. Urquijo, 23-05-10
                        , StrippingB2XuMuNuConf().K_line()
                        , StrippingB2XuMuNuConf().Pi_line()
                        , StrippingB2XuMuNuConf().PiSS_line()
                        , StrippingB2XuMuNuConf().Rho_line()
                        , StrippingB2XuMuNuConf().RhoWS_line()
                        , StrippingB2XuMuNuConf().KSS_line()
                        , StrippingB2XuMuNuConf().Kstar_line()
                        , StrippingB2XuMuNuConf().KstarSS_line()
		    ] )


from StrippingSelections import StrippingB0q2DplusMuX #B0q2DplusMuXAllLinesConf, confdict

confB0q2DplusMuX = StrippingB0q2DplusMuX.B0q2DplusMuXAllLinesConf(StrippingB0q2DplusMuX.confdict)

stream.appendLines( confB0q2DplusMuX.Lines ) #StrippingB0q2DplusMuX.B0q2DplusMuXAllLinesConf(StrippingB0q2DplusMuX.confdict))
