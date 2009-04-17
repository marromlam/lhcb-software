from Configurables import HltANNSvc 

# quick hack to make sure Hlt2Global is OK...
Hlt2ID = HltANNSvc().Hlt2SelectionID
if "Hlt2Global" not in Hlt2ID : Hlt2ID.update( {  "Hlt2Global" : 2 } )

HltANNSvc().Hlt1SelectionID = {
    # Global
    "Hlt1Global"                            :    1,
    # L0 (prescaled)
    "Hlt1L0MuonDecision"                    :   10,
    "Hlt1L0MuonNoGlobDecision"              :   11,
    "Hlt1L0DiMuonDecision"                  :   12,
    "Hlt1L0Muon,lowMultDecision"            :   13,
    "Hlt1L0DiMuon,lowMultDecision"          :   14,
    "Hlt1L0ElectronDecision"                :   15,
    "Hlt1L0PhotonDecision"                  :   16,
    "Hlt1L0HadronDecision"                  :   17,
    "Hlt1L0GlobalPi0Decision"               :   18,
    "Hlt1L0LocalPi0Decision"                :   19,
    # Misc...
    "Hlt1IncidentDecision"                  :   30,
    # LU
    "Hlt1LumiDecision"                      :   40, 
    "Hlt1IgnoringLumiDecision"              :   41,
    "Hlt1LumiNoBeamDecision"                :   45, 
    "Hlt1LumiBeamCrossingDecision"          :   46, 
    "Hlt1LumiSingleBeamLeftDecision"        :   47, 
    "Hlt1LumiSingleBeamRightDecision"       :   48,
    # BG
    "Hlt1BeamGasDecision"                   :   50, 
    "Hlt1BeamGasLeftDecision"               :   51,
    "Hlt1BeamGasRightDecision"              :   52,
    "Hlt1BeamGasCrossingDecision"           :   53,
    # VE
    "Hlt1VeloClosingDecision"               :   60,
    "Hlt1VeloASideDecision"                 :   61,
    "Hlt1VeloCSideDecision"                 :   62,
    # PA
    "Hlt1RandomDecision"                    :   70,
    "Hlt1PhysicsDecision"                   :   71,
    "Hlt1L0ForcedDecision"                  :   72,
    "Hlt1RawBankConversionDecision"         :   73,
    # XP
    "Hlt1XPressDecision"                    :   80,
    # MU
    "Hlt1SingleMuonNoIPL0Decision"          :  100,
    "Hlt1SingleMuonNoIPGECDecision"         :  101,
    "Hlt1SingleMuonIPCL0Decision"           :  105,
    "Hlt1SingleMuonIPCGECDecision"          :  106,
    "Hlt1DiMuonNoIPL0DiDecision"            :  110,
    "Hlt1DiMuonNoIP2L0Decision"             :  111,
    "Hlt1DiMuonNoIPL0SegDecision"           :  112,
    "Hlt1DiMuonNoIPL0GECDecision"           :  113,
    "Hlt1DiMuonNoIPGECSegDecision"          :  114,
    "Hlt1DiMuonIPCL0DiDecision"             :  120,
    "Hlt1DiMuonIPC2L0Decision"              :  121,
    "Hlt1DiMuonIPCL0SegDecision"            :  122,
    "Hlt1DiMuonIPCL0GRCDecision"            :  123,
    "Hlt1DiMuonIPCGECSegDecision"           :  124,
    "Hlt1DiMuonNoPVL0DiDecision"            :  130,
    "Hlt1DiMuonNoPV2L0Decision"             :  131,
    "Hlt1DiMuonNoPVL0SegDecision"           :  132,
    #
    "Hlt1MuTrackDecision"                   :  150,
    "Hlt1MuTrack4JPsiDecision"              :  151,
    # HA
    "Hlt1SingleHadronDecision"              :  200,
    "Hlt1DiHadronDecision"                  :  201,
    # PH
    "Hlt1PhotonDecision"                    :  300,
    "Hlt1PhoFromEleDecision"                :  301,
    "Hlt1PhotonFromEleDecision"             :  302,
    # EL
    "Hlt1SingleElectronDecision"            :  400,
    "Hlt1ElectronFromPi0TrackNoIPDecision"  :  401,
    "Hlt1ElectronFromPi0TrackWithIPDecision":  402,
    "Hlt1ElectronTrackDecision"             :  403,
    "Hlt1ElectronTrackNoIPDecision"         :  404,
    "Hlt1ElectronTrackWithIPDecision"       :  405,
    "Hlt1SingleElectronFromPi0Decision"     :  406,
    #
    # Autogenerated Decisions start at 1K
    #
    # intermediate selections: 10000 -- 20000
    # 'All' L0 candidates ; 
    'Hlt1L0AllMuonCandidates'               :10000,
    'Hlt1L0AllElectronCandidates'           :10010,
    'Hlt1L0AllPhotonCandidates'             :10011,
    'Hlt1L0AllHadronCandidates'             :10012,
    'Hlt1L0AllLocalPi0Candidates'           :10013,
    'Hlt1L0AllGlobalPi0Candidates'          :10014,
    #
    'RZVelo'                                :10100,
    'PV2D'                                  :10101,
    #
    # 11K - 20K : added automatically by configuration...
    # 50K+ : used by Hlt2
}

#/ generated by parsing the logfile, of a job _without_ explicitly specified 
#/ InfoID, with:
#/ awk 'BEGIN{n="foo"}/autogenerated: InfoID/{if (substr($6,0,length(n))!=n) {n=$6; i =(1+int(i/100))*100} ; print sprintf( "    %-40s: %4d,","\""$6"\"",++i)}' logfile

##
## InfoID below 1000 is reserved for the tracking reconstruction group (JAH 020409)
## 
HltANNSvc().InfoID = {
    "AntiEleConf"                           : 1101,
    "Calo2DChi2"                            : 1201,
    "Calo2DChi2_Hlt1DiHadronTFL0Hadrons"    : 1202,
    "Calo2DChi2_Hlt1SingleHadronTFL0Hadrons": 1203,
    "Calo3DChi2"                            : 1301,
    "Calo3DChi2_Hlt1DiHadronTFL0Hadrons"    : 1302,
    "Calo3DChi2_Hlt1SingleHadronTFL0Hadrons": 1303,
    "Chi2Mu"                                : 1311,
    "Chi2OverN"                             : 1312,
    "DOCA"                                  : 1401,
    "DOCA_Hlt1MuTrackTFMuon"                : 1402,
    "DOCA_Hlt1PrepElectronTFForward"        : 1403,
    "DeltaP"                                : 1501,
    "DoShareM3"                             : 1601,
    "DoShareM3_Hlt1SingleMuonNoPVPrepTMVeloT": 1602,
    "DoShareM3_Hlt1SingleMuonPrepTMVeloT"   : 1603,
    "Ecal2DChi2"                            : 1701,
    "Ecal2DChi2_Hlt1L0ElectronDecision"     : 1702,
    "Ecal3DChi2"                            : 1801,
    "Ecal3DChi2_Hlt1L0ElectronDecision"     : 1802,
    "FitChi2OverNdf"                        : 1901,
    "FitIP"                                 : 1001,
    "FitIP_PV2D"                            : 1002,
    "FitTrack"                              : 1003,
    "FitVertexAngle"                        :11101,
    "FitVertexDOCA"                         :11201,
    "FitVertexDimuonMass"                   :11301,
    "FitVertexDz"                           :11401,
    "FitVertexDz_PV2D"                      :11402,
    "FitVertexMaxChi2OverNdf"               :11501,
    "FitVertexMinIP"                        :11601,
    "FitVertexMinIP_PV2D"                   :11602,
    "FitVertexMinPT"                        :11701,
    "FitVertexPointing"                     :11801,
    "FitVertexPointing_PV2D"                :11802,
    "HltGuidedForward"                      :11901,
    "HltTrackFit"                           : 2001,
    "IP"                                    : 2101,
    "IP_PV2D"                               : 2102,
    "IsMuon"                                : 2201,
    "IsPhoton"                              : 2301,
    "L0ConfirmWithT"                        : 2401,
    "L0ET"                                  : 2501,
    "MatchIDsFraction"                      : 2601,
    "MatchIDsFraction_Hlt1DiHadronTFGuidedForward": 2602,
    "PT"                                    : 2701,
    "PT0"                                   : 2702,
    "PatForwardTool"                        : 2801,
    "RZVeloTMatch"                          : 2901,
    "RZVeloTMatch_Hlt1DiMuonIPL0DiDiMuonPrepTFTConf": 2902,
    "RZVeloTMatch_Hlt1DiMuonNoPVL0DiMuonPrepTFTConf": 2903,
    "RZVeloTMatch_Hlt1DiMuonNoIPL0DiMuonPrepTFTConf": 2904,
    "RZVeloTMatch_Hlt1SingleMuonNoPVSegPrepTFTConf": 2905,
    "RZVeloTMatch_Hlt1SingleMuonSegPrepTFTConf"   : 2906,
    "RZVeloTMatch_Hlt1SingleMuonNoPVPrepNoTFTConf": 2907,
    "RZVeloTMatch_Hlt1SingleMuonPrepTFTConf"      : 2908,
    "SumPT"                                 : 3001,
    "Tf::PatVeloSpaceTool"                  : 3101,
    "VeloCalo3DChi2"                        : 3201,
    "VertexAngle"                           : 3301,
    "VertexDimuonMass"                      : 3401,
    "VertexDz"                              : 3501,
    "VertexDz_PV2D"                         : 3502,
    "VertexMatchIDsFraction"                : 3601,
    "VertexMatchIDsFraction_Hlt1DiMuonIPL0DiDiMuonPrepPVM1L0DiMuon": 3602,
    "VertexMatchIDsFraction_Hlt1DiMuonNoPVL0DiDiMuonPrepVM1L0DiMuon": 3603,
    "VertexMatchIDsFraction_Hlt1DiMuonNoIPL0DiDiMuonPrepVM1L0DiMuon": 3604,
    "VertexMinIP"                           : 3701,
    "VertexMinIP_PV2D"                      : 3702,
    "VertexMinPT"                           : 3801,
    "VertexNumberOfASideTracks"             : 3901,
    "VertexNumberOfCSideTracks"             : 4001,
    "VertexPointing"                        : 4101,
    "VertexPointing_PV2D"                   : 4102,
    "chi2_PatMatch"                         : 4201,
    "rIP"                                   : 4301,
    "rIP_PV2D"                              : 4302,
}
