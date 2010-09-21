from Gaudi.Configuration import *

from Configurables import GaudiSequencer
from Configurables import CombineParticles, PhysDesktop, FilterToFixOppositeBFractions

SeqSelBs2DsH = GaudiSequencer("SeqSelBs2DsH")

#FilterToFixOppositeBFractionsAlg=FilterToFixOppositeBFractions("FilterToFixOppositeBFractions")
#SeqSelBs2DsH.Members += ["FilterToFixOppositeBFractions"]
#FilterToFixOppositeBFractionsAlg.ActivateCorrection = True;

#SeqSelBs2DsH.Members += ["PreLoadParticles/StdLoosePions"
#                         ,"PreLoadParticles/StdLooseKaons"]
######################
#presel cuts

AllKaonCuts =  "(PT>200*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)>2)"
AllPionCuts =  "(PT>200*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)>2)"
BachelorKaonCut = "(PT>200*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)>2)"
BachelorPionCut = "(PT>200*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)>2)"
DsMassCut = "(MM>1768.3*MeV) & (MM<2168.3*MeV) & "
BsMassCut = "(MM>4869.6*MeV) & (MM<5869.6*MeV) & "
#DsMassCut = "(ADAMASS('D_s+')<200*MeV)"
#BsMassCut = "(ADAMASS('B_s0')<500*MeV)";
DsPropertyCut = DsMassCut+"(PT>1*GeV) & (VFASPF(VCHI2/VDOF)<16.67) & (MIPCHI2DV(PRIMARY)>6)"
BsPropertyCut = BsMassCut+"((VFASPF(VCHI2/VDOF)<20) & (BPVIPCHI2()<81) & (BPVVDCHI2>9) & (BPVDIRA>0.9))"

#DsPropertyCut = DsMassCut+"(PT>1*GeV) & (VFASPF(VCHI2/VDOF)<6.67) & (MIPCHI2DV(PRIMARY)>9)"
#BsPropertyCut = BsMassCut+"((VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<64) & (BPVVDCHI2>9) & (BPVDIRA>0.999))"
#
# Ds PreSelection
CombineDs2KKPiLoosePreSelAlg = CombineParticles("CombineDs2KKPiLoosePreSel")   
SeqSelBs2DsH.Members += [CombineDs2KKPiLoosePreSelAlg]
CombineDs2KKPiLoosePreSelAlg.InputLocations = ["Phys/StdLoosePions", "Phys/StdLooseKaons"]
CombineDs2KKPiLoosePreSelAlg.DecayDescriptor = "[D_s- -> K+ K- pi-]cc"
CombineDs2KKPiLoosePreSelAlg.DaughtersCuts = {"K+"  : AllKaonCuts , "pi+" : AllPionCuts }
#CombineDs2KKPiLoosePreSelAlg.CombinationCut = DsMassCut
CombineDs2KKPiLoosePreSelAlg.MotherCut = DsPropertyCut

###############################################################################################
# Bs PreSelection for Bs2DsK
PreSelCombBs2DsKAlg = CombineParticles("PreSelCombBs2DsK")
SeqSelBs2DsH.Members += [PreSelCombBs2DsKAlg]
PreSelCombBs2DsKAlg.InputLocations = ["Phys/StdLooseKaons", "Phys/CombineDs2KKPiLoosePreSel"]
PreSelCombBs2DsKAlg.DecayDescriptor = "[B_s0 -> D_s- K+]cc"
PreSelCombBs2DsKAlg.DaughtersCuts = {"K+" : BachelorKaonCut }
#PreSelCombBs2DsKAlg.CombinationCut = BsMassCut
PreSelCombBs2DsKAlg.MotherCut = BsPropertyCut

