__author__ = 'Paul Schaack'
__date__ = '11/10/2010'
__version__ = '$Revision: 1.0 $'

"""
Stripping selection for Bs -> Mu Mu Phi ( -> K K).
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import Selection, AutomaticData
from StrippingSelections.Utils import checkConfig


#################
#
#  Define Cuts here
#
#################


defaultConfig = {
    'BsIPCHI2'             : 9.0           # dimensionless
    , 'BsLT'               : 0.0002        # ns
    , 'BsVertexCHI2'       : 40.0          # dimensionless
    , 'KaonPIDK'           : 0             # dimensionless
    , 'KaonMINIPCHI2'      : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 9.0           # dimensionless
}

#################
#
#  Make line here
#
#################

class StrippingBs2MuMuPhiConf(object):
    __config_keys__ = (
        'BsIPCHI2'
        , 'BsLT'
        , 'BsVertexCHI2'
        , 'KaonPIDK'
        , 'KaonMINIPCHI2'
        , 'MuonMINIPCHI2'
    )

    def __init__(self, config, name="Bs2MuMuPhi"):
        """
        Creates Bs Selection object
        """
        checkConfig(StrippingBs2MuMuPhiConf.__config_keys__, config)

        self.name = name
        self.Muons = self.__Muons__(config)
        self.Kaons = self.__Kaons__(config)
        self.Bs = self.selBs(self.Muons, self.Kaons, config)
        self.line = None

    def __Muons__(self, conf):
        """
        Filter muons from StdLooseMuons
        """  
        _muons = AutomaticData(Location = 'Phys/StdLooseMuons')
        _filter = FilterDesktop("Filter_"+self.name+"_Muons",
                                Code = self.__MuonCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_Muons",
                         RequiredSelections = [ _muons ] ,
                         Algorithm = _filter)

        return _sel
        
    def __Kaons__(self, conf):
        """
        Filter kaons from StdLooseKaons
        """  
        _kaons = AutomaticData(Location = 'Phys/StdLooseKaons')
        _filter = FilterDesktop("Filter_"+self.name+"_Kaons",
                                Code = self.__KaonCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_Kaons",
                         RequiredSelections = [ _kaons ] ,
                         Algorithm = _filter)

        return _sel

    def __KaonCuts__(self, conf):
        """
        Returns the Kaon cut string
        """
        _KaonCuts = """
        (PIDK > %(KaonPIDK)s ) &
        (MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s )
        """ % conf
        return _KaonCuts

    def __MuonCuts__(self, conf):
        """
        Returns the Muon cut string
        """
        _MuonCuts = """
        (MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s )
        """ % conf
        return _MuonCuts

    def __BsCuts__(self, conf):
        """
        Returns the Bs cut string
        """
        _BsCuts = """
        (VFASPF(VCHI2) < %(BsVertexCHI2)s ) &
        (BPVLTIME( %(BsIPCHI2)s ) > %(BsLT)s *ns)
        """ % conf
        return _BsCuts

    def selBs(self, Muons, Kaons, conf):
        """
        Make and return a Bs selection
        """      
        _bs2KKmumu = CombineParticles("Combine_"+self.name)
        _bs2KKmumu.DecayDescriptor = "B_s0 -> K+ K- mu+ mu-"
        _bs2KKmumu.CombinationCut = "(ADAMASS('B_s0') < 1000.0 *MeV) & (AM12 < 1070.0 *MeV)"
        _bs2KKmumu.MotherCut = self.__BsCuts__(conf)
        
        SelBS2KKMUMU = Selection( "Selection_"+self.name,
                                  Algorithm = _bs2KKmumu,
                                  RequiredSelections = [ Muons, Kaons ] )
        return SelBS2KKMUMU

    def lines(self):
        """
        Return signal line
        """
        if None == self.line:
            self.line = StrippingLine(self.name+"_line",
                                      prescale = 1,
                                      algos = [ self.Bs ]
                                      )
        return [ self.line ] 

