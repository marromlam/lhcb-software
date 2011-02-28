###
#
#  Hlt2 Charmonium -> p pbar
#  @ author Serezha.Barsuk@cern.ch
#  @ author Jibo.He@cern.ch
#  @ date 2010-03-19
#
###

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiProtonLinesConf(HltLinesConfigurableUser):
    """
    
    """
    
    __slots__ = { 'Prescale'           : {    'Hlt2DiProton'                   : 1.  
                                            , 'Hlt2DiProtonTF'                 : 0.005  # of 200
                                            , 'Hlt2DiProtonLowMult'            : 1.
                                            , 'Hlt2DiProtonLowMultTF'          : 0.005     
                                            }
                  
                  ,'Postscale'         : {    'Hlt2DiProton'                   : 1.  
                                            , 'Hlt2DiProtonTF'                 : 1.
                                            , 'Hlt2DiProtonLowMult'            : 1.
                                            , 'Hlt2DiProtonLowMultTF'          : 1.
                                            }
                  
                  , 'SpdMult'            :   600.
                  , 'DiProton_Hlt1Req'   :  "HLT_PASS('Hlt1DiProtonDecision')"
                  # Track Fitted
                  , 'TFProtonPT'         :  1900.   # MeV
                  , 'TFProtonTrkChi2'    :     4.   
                  , 'TFCombUpperMass'    :  4550.   # MeV, before Vtx fit
                  , 'TFCombLowerMass'    :  2850.   # MeV, before Vtx fit
                  , 'TFCombPT'           :  5800.   # MeV, before Vtx fit
                  , 'TFUpperMass'        :  4500.   # MeV, after Vtx fit
                  , 'TFLowerMass'        :  2900.   # MeV, after Vtx fit
                  , 'TFCCbarPT'          :  6000.   # MeV, after Vtx fit 
                  , 'TFVtxCHI2'          :     9.   # dimensionless
                  # Track Fitted & RichPID
                  , 'TFRichProtonPIDppi'   :  10.   # CombDLL(p-pi)
                  , 'TFRichProtonPIDpK'    :   5.   # CombDLL(p-K)

                  , 'LowMult_SpdMult'            :    20.
                  , 'DiProtonLowMult_Hlt1Req'    :  "HLT_PASS('Hlt1DiProtonLowMultDecision')"
                  # Track Fitted
                  , 'LowMult_TFProtonPT'         :   500.   # MeV
                  , 'LowMult_TFProtonTrkChi2'    :     5.   
                  , 'LowMult_TFCombLowerMass'    :  2850.   # MeV, before Vtx fit
                  , 'LowMult_TFLowerMass'        :  2900.   # MeV, after Vtx fit
                  , 'LowMult_TFVtxCHI2'          :     9.   # dimensionless
                  # Track Fitted & RichPID
                  , 'LowMult_TFRichProtonPIDppi'   :  10.   # CombDLL(p-pi)
                  , 'LowMult_TFRichProtonPIDpK'    :   5.   # CombDLL(p-K)                
                  
                  
                  , 'HltANNSvcID'      : {  'DiProton'                   : 51000
                                           ,'DiProtonTF'                 : 51002
                                           ,'DiProtonLowMult'            : 51005
                                           ,'DiProtonLowMultTF'          : 51006
                                           }
                  }
    
    def __apply_configuration__(self):
        self.__makeHlt2DiProtonLines()
        self.__makeHlt2DiProtonLowMultLines()
        
    def __makeHlt2DiProtonLines(self):        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiProtonDecision" : self.getProp('HltANNSvcID')['DiProton'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiProtonTFDecision" : self.getProp('HltANNSvcID')['DiProtonTF'] } )


        """
        #------------------------
        #  Hlt1 Requirements
        #------------------------
        """
        Hlt1Req = self.getProp("DiProton_Hlt1Req")
        
        if not Hlt1Req:
            Hlt1Req= None        
            
            
        #------------------------------------
        # Track Fitted
        #------------------------------------
        TFProtonCut = "(PT> %(TFProtonPT)s *MeV) & (TRCHI2DOF < %(TFProtonTrkChi2)s)" % self.getProps()
        TFCombCut = "(in_range( %(TFCombLowerMass)s *MeV, AM, %(TFCombUpperMass)s *MeV)) & (APT> %(TFCombPT)s *MeV)" % self.getProps()
        TFMomCut = "(VFASPF(VCHI2PDOF)< %(TFVtxCHI2)s) & (in_range( %(TFLowerMass)s *MeV, MM, %(TFUpperMass)s *MeV)) & (PT> %(TFCCbarPT)s *MeV)" % self.getProps()
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons
        TFCombine = Hlt2Member( CombineParticles
                                , "TFCombine"
                                , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                , DaughtersCuts = { "p+" : TFProtonCut }
                                , CombinationCut = TFCombCut
                                , MotherCut = TFMomCut
                                , Inputs = [ BiKalmanFittedProtons ]
                                , InputPrimaryVertices = "None"
                                , UseP2PVRelations = False
                                )

        #------------------------------------
        # Track Fitted & RichPID
        #------------------------------------
        TFRichProtonPID = "((PIDp-PIDpi) > %(TFRichProtonPIDppi)s) & ((PIDp-PIDK) > %(TFRichProtonPIDpK)s)" % self.getProps()   
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichProtons
        TFRichCombine = Hlt2Member( CombineParticles
                                    , "TFRichCombine"
                                    , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                    , DaughtersCuts = { "p+" : TFProtonCut+" & "+TFRichProtonPID }
                                    , CombinationCut = TFCombCut
                                    , MotherCut = TFMomCut
                                    , Inputs = [ BiKalmanFittedRichProtons ]
                                    , InputPrimaryVertices = "None"
                                    , UseP2PVRelations = False
                                    )

        #------------------------------------
        # Inclusive DiProton complete
        #------------------------------------
        line = Hlt2Line('DiProton'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps()
                        , HLT  = Hlt1Req
                        , algos = [ BiKalmanFittedProtons
                                    , TFCombine
                                    , BiKalmanFittedRichProtons
                                    , TFRichCombine
                                    ]
                        , postscale = self.postscale
                        )
        
        #------------------------------------
        # Inclusive TF
        #------------------------------------
        line = Hlt2Line('DiProtonTF'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps()
                        , HLT  = Hlt1Req
                        , algos = [ BiKalmanFittedProtons
                                    , TFCombine
                                    ]
                        , postscale = self.postscale
                        )       




    def __makeHlt2DiProtonLowMultLines(self):        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiProtonLowMultDecision" : self.getProp('HltANNSvcID')['DiProtonLowMult'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiProtonLowMultTFDecision" : self.getProp('HltANNSvcID')['DiProtonLowMultTF'] } )

        """
        #------------------------
        #  Hlt1 Requirements
        #------------------------
        """
        Hlt1Req = self.getProp("DiProtonLowMult_Hlt1Req")
        
        if not Hlt1Req:
            Hlt1Req= None       
        
        #------------------------------------
        # Track Fitted
        #------------------------------------
        TFProtonCut = "(PT> %(LowMult_TFProtonPT)s *MeV) & (TRCHI2DOF < %(LowMult_TFProtonTrkChi2)s)" % self.getProps()
        TFCombCut = "(AM>%(LowMult_TFCombLowerMass)s *MeV)" % self.getProps()
        TFMomCut = "(VFASPF(VCHI2PDOF)< %(LowMult_TFVtxCHI2)s) & (MM> %(LowMult_TFLowerMass)s *MeV)" % self.getProps()
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons
        TFCombine = Hlt2Member( CombineParticles
                                , "TFCombine"
                                , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                , DaughtersCuts = { "p+" : TFProtonCut }
                                , CombinationCut = TFCombCut
                                , MotherCut = TFMomCut
                                , Inputs = [ BiKalmanFittedProtons ]
                                , InputPrimaryVertices = "None"
                                , UseP2PVRelations = False
                                )

        #------------------------------------
        # Track Fitted & RichPID
        #------------------------------------
        TFRichProtonPID = "((PIDp-PIDpi) > %(LowMult_TFRichProtonPIDppi)s) & ((PIDp-PIDK) > %(LowMult_TFRichProtonPIDpK)s)" % self.getProps()   
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichLowPTProtons
        TFRichCombine = Hlt2Member( CombineParticles
                                    , "TFRichCombine"
                                    , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                    , DaughtersCuts = { "p+" : TFProtonCut+" & "+TFRichProtonPID }
                                    , CombinationCut = TFCombCut
                                    , MotherCut = TFMomCut
                                    , Inputs = [ BiKalmanFittedRichLowPTProtons ]
                                    , InputPrimaryVertices = "None"
                                    , UseP2PVRelations = False
                                    )

        #------------------------------------
        # Inclusive DiProtonLowMult complete
        #------------------------------------
        line = Hlt2Line('DiProtonLowMult'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(LowMult_SpdMult)s )" % self.getProps()
                        , HLT  = Hlt1Req
                        , algos = [ BiKalmanFittedProtons
                                    , TFCombine
                                    , BiKalmanFittedRichLowPTProtons
                                    , TFRichCombine
                                    ]
                        , postscale = self.postscale
                        )
        
        #------------------------------------
        # Inclusive TF
        #------------------------------------
        line = Hlt2Line('DiProtonLowMultTF'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(LowMult_SpdMult)s )" % self.getProps()
                        , HLT  = Hlt1Req
                        , algos = [ BiKalmanFittedProtons
                                    , TFCombine
                                    ]
                        , postscale = self.postscale
                        )           
