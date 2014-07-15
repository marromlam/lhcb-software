from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmRareDecayLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
        'D02MuMuMinDaughterPt'            : 1.0 # GeV
      , 'D02MuMuMinLifeTime'              : 0.1 # ps
      , 'D02MuMuMinIPChi2'                : 4.0
      , 'D02PiPiForD02MuMuMinLifeTime'    : 0.1 # ps (todo: remove)
      , 'Prescale' : {
          'Hlt2CharmRareDecayD02MuMu'           : 1.0
        , 'Hlt2CharmRareDecayD02PiPiForD02MuMu' : 0.1
        }
      , 'HltANNSvcID' : {
          'Hlt2CharmRareDecayD02MuMuDecision'           : 50951
        , 'Hlt2CharmRareDecayD02PiPiForD02MuMuDecision' : 50952
        }
    }

    def __apply_configuration__(self) :

        from HltLine.HltLine import Hlt2Member, Hlt2Line
        from Configurables import HltANNSvc, CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons, BiKalmanFittedPions
        from HltTracking.HltPVs import PV3D

        D0DaughterCut = "(PT > %(D02MuMuMinDaughterPt)s * GeV) & (MIPCHI2DV(PRIMARY) > %(D02MuMuMinIPChi2)s)" % self.getProps()
        D02MuMuCombinationCut = "(ADAMASS('D0') < 100 * MeV)"
        D02PiPiCombinationCut = "(ADAMASS('D0') < 50 * MeV)"
        D02MuMuMotherCut = "(VFASPF(VCHI2PDOF) < 25) & (BPVLTIME(25) > %(D02MuMuMinLifeTime)s * ps)" % self.getProps()
        D02PiPiMotherCut = "(VFASPF(VCHI2PDOF) < 25) & (BPVLTIME(25) > %(D02MuMuMinLifeTime)s * ps)" % self.getProps()

        D02MuMuCombine = Hlt2Member(CombineParticles
                                    , 'D02MuMuCombine'
                                    , DecayDescriptor = 'D0 -> mu+ mu-'
                                    , Inputs = [ BiKalmanFittedMuons ]
                                    , DaughtersCuts = { "mu+" : D0DaughterCut }
                                    , CombinationCut = D02MuMuCombinationCut
                                    , MotherCut = D02MuMuMotherCut)
        lineCharmRareDecayD02MuMu = \
          Hlt2Line('CharmRareDecayD02MuMu'
                   , prescale = self.prescale
                   , algos = [ PV3D('Hlt2'), BiKalmanFittedMuons, D02MuMuCombine ]
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmRareDecayD02MuMuDecision" : self.getProp('HltANNSvcID')['Hlt2CharmRareDecayD02MuMuDecision'] } )

        D02PiPiForD02MuMuCombine = Hlt2Member(CombineParticles
                                              , 'D02PiPiForD02MuMuCombine'
                                              , DecayDescriptor = 'D0 -> pi+ pi-'
                                              , Inputs = [ BiKalmanFittedPions ]
                                              , DaughtersCuts = { "pi+" : D0DaughterCut }
                                              , CombinationCut = D02PiPiCombinationCut
                                              , MotherCut = D02PiPiMotherCut)
        lineCharmRareDecayD02PiPiForD02MuMu = \
          Hlt2Line('CharmRareDecayD02PiPiForD02MuMu'
                   , prescale = self.prescale
                   , algos = [ PV3D('Hlt2'), BiKalmanFittedPions, D02PiPiForD02MuMuCombine ]
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmRareDecayD02PiPiForD02MuMuDecision" : self.getProp('HltANNSvcID')['Hlt2CharmRareDecayD02PiPiForD02MuMuDecision'] } )
