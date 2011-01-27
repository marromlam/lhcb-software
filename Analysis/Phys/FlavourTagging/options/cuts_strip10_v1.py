########################################################################
#
# Tagging cuts optimized for Stripping10_v1
#
# @author Marc Grabalosa
# @date 2010-09-26
########################################################################

from Gaudi.Configuration import *
from Configurables import BTagging, BTaggingTool, TaggerMuonTool, TaggerElectronTool, TaggerKaonSameTool, TaggerKaonOppositeTool, TaggerPionSameTool, TaggerVertexChargeTool

tag = BTagging ('BTagging')
tag.addTool ( BTaggingTool )

tag.BTaggingTool.IPPU_cut = 3.0
tag.BTaggingTool.distphi_cut = 0.005

tag.BTaggingTool.addTool ( CombineTaggersProbability )
tag.BTaggingTool.CombineTaggersProbability.P0_Cal_OS = 0.
tag.BTaggingTool.CombineTaggersProbability.P1_Cal_OS = 1.
tag.BTaggingTool.CombineTaggersProbability.Eta_Cal_OS = 0.

tag.BTaggingTool.addTool ( TaggerMuonTool )
tag.BTaggingTool.TaggerMuonTool.Muon_Pt_cut   = 1100
tag.BTaggingTool.TaggerMuonTool.Muon_P_cut    = 0.0 
tag.BTaggingTool.TaggerMuonTool.Muon_IPs_cut  = 0.0
tag.BTaggingTool.TaggerMuonTool.Muon_lcs_cut  = 2.2
tag.BTaggingTool.TaggerMuonTool.Muon_PIDm_cut = 2.0
tag.BTaggingTool.TaggerMuonTool.Muon_ipPU_cut = 3.0
tag.BTaggingTool.TaggerMuonTool.Muon_distPhi_cut = 0.005
tag.BTaggingTool.TaggerMuonTool.ProbMin_muon  = 0.0
tag.BTaggingTool.TaggerMuonTool.AverageOmega  = 0.33
tag.BTaggingTool.TaggerMuonTool.Muon_P0_Cal   = 0.
tag.BTaggingTool.TaggerMuonTool.Muon_P1_Cal   = 1.
tag.BTaggingTool.TaggerMuonTool.Muon_Eta_Cal  = 0.

tag.BTaggingTool.addTool ( TaggerElectronTool )
tag.BTaggingTool.TaggerElectronTool.Ele_Pt_cut    = 1000.
tag.BTaggingTool.TaggerElectronTool.Ele_P_cut     = 0.0
tag.BTaggingTool.TaggerElectronTool.Ele_lcs_cut   = 5.
tag.BTaggingTool.TaggerElectronTool.Ele_IPs_cut   = 2.
tag.BTaggingTool.TaggerElectronTool.Ele_ghost_cut =-999.
tag.BTaggingTool.TaggerElectronTool.VeloChargeMin = 0.0 
tag.BTaggingTool.TaggerElectronTool.VeloChargeMax = 1.6
tag.BTaggingTool.TaggerElectronTool.EoverP        = 0.6
tag.BTaggingTool.TaggerElectronTool.Ele_PIDe_cut  = 4.
tag.BTaggingTool.TaggerElectronTool.Ele_ipPU_cut  = 3.
tag.BTaggingTool.TaggerElectronTool.Ele_distPhi_cut  = 0.03
tag.BTaggingTool.TaggerElectronTool.ProbMin_ele   = 0.0
tag.BTaggingTool.TaggerElectronTool.AverageOmega  = 0.33
tag.BTaggingTool.TaggerElectronTool.Ele_P0_Cal    = 0.
tag.BTaggingTool.TaggerElectronTool.Ele_P1_Cal    = 1.
tag.BTaggingTool.TaggerElectronTool.Ele_Eta_Cal   = 0.

tag.BTaggingTool.addTool ( TaggerKaonOppositeTool )
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Pt_cut   = 800
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P_cut    = 4000
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_IPs_cut  = 4.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_IP_cut   = 1.5
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_LCS_cut  = 2.7
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_PIDk     = 2.0
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_PIDkp    = -1.0
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ghost_cut= -999.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ipPU_cut = 6.0
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_distPhi_cut = 0.005
tag.BTaggingTool.TaggerKaonOppositeTool.ProbMin_kaon  = 0.51
tag.BTaggingTool.TaggerKaonOppositeTool.AverageOmega  = 0.33
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P0_Cal   = 0.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P1_Cal   = 1.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Eta_Cal  = 0.

tag.BTaggingTool.addTool ( TaggerKaonSameTool )
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Pt_cut  = 450
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_P_cut   = 4000
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_IP_cut  = 3.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Phi_cut = 1.1
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Eta_cut = 1.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_dQ_cut  = 1600
tag.BTaggingTool.TaggerKaonSameTool.KaonS_LCS_cut    = 5.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSPID_kS_cut  = 1.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSPID_kpS_cut = -1.0
tag.BTaggingTool.TaggerKaonSameTool.KaonS_ipPU_cut   = 3.0
tag.BTaggingTool.TaggerKaonSameTool.KaonS_distPhi_cut   = 0.005
tag.BTaggingTool.TaggerKaonSameTool.ProbMin_kaonS    = 0.0
tag.BTaggingTool.TaggerKaonSameTool.AverageOmega     = 0.33

tag.BTaggingTool.addTool ( TaggerPionSameTool )
tag.BTaggingTool.TaggerPionSameTool.PionSame_Pt_cut       = 750
tag.BTaggingTool.TaggerPionSameTool.PionSame_P_cut        = 5000
tag.BTaggingTool.TaggerPionSameTool.PionSame_IPs_cut      = 3.5
tag.BTaggingTool.TaggerPionSameTool.PionS_LCS_cut         = 5.0
tag.BTaggingTool.TaggerPionSameTool.PionSame_dQ_cut       = 2500
tag.BTaggingTool.TaggerPionSameTool.PionSame_dQ_extra_cut = 1500 
tag.BTaggingTool.TaggerPionSameTool.Pion_ghost_cut        = -999.
tag.BTaggingTool.TaggerPionSameTool.PionSame_ipPU_cut     = 3.
tag.BTaggingTool.TaggerPionSameTool.PionSame_distPhi_cut  = 0.005 
tag.BTaggingTool.TaggerPionSameTool.PionSame_PIDNoK_cut   = 3.0
tag.BTaggingTool.TaggerPionSameTool.PionSame_PIDNoP_cut   = 10.0
tag.BTaggingTool.TaggerPionSameTool.PionProbMin           = 0.53
tag.BTaggingTool.TaggerPionSameTool.AverageOmega          = 0.41
tag.BTaggingTool.TaggerPionSameTool.PionSame_P0_Cal       = 0.
tag.BTaggingTool.TaggerPionSameTool.PionSame_P1_Cal       = 1.
tag.BTaggingTool.TaggerPionSameTool.PionSame_Eta_Cal      = 0.

tag.BTaggingTool.addTool ( TaggerVertexChargeTool )
tag.BTaggingTool.TaggerVertexChargeTool.AverageOmega  = 0.41
tag.BTaggingTool.TaggerVertexChargeTool.PowerK        = 0.4
tag.BTaggingTool.TaggerVertexChargeTool.MinimumVCharge= 0.17
tag.BTaggingTool.TaggerVertexChargeTool.ProbMin_vtx   = 0.53
tag.BTaggingTool.TaggerVertexChargeTool.P0            = 5.255669e-01
tag.BTaggingTool.TaggerVertexChargeTool.P1            =-3.251661e-01
tag.BTaggingTool.TaggerVertexChargeTool.Gt075         = 0.35
tag.BTaggingTool.TaggerVertexChargeTool.TracksEq2     = 0.4141
tag.BTaggingTool.TaggerVertexChargeTool.TracksGt2     = 0.3250
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P0_Cal    = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P1_Cal    = 1.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Eta_Cal   = 0.
