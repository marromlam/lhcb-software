
__author__ = 'Regis Lefevre'
__date__ = '2011.02.24'
__version__ = '$Revision: 1.3 $'

'''
Stripping selection for B -> h h pi0
'''
#################################################################
#  This strip is used both for Bd -> pi pi pi0 and Bs -> K pi pi0
#  B Mass window : 4200 to 6400 MeV/c2 
#  2 lines : one for merged, one for resolved pi0
#################################################################

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions,StdLooseMergedPi0,StdLooseResolvedPi0

class StrippingB2HHPi0Conf(LineBuilder) :

    __configuration_keys__ = ( 'PiMinPT'
                               ,'PiMinP'
                               ,'PiMinTrackProb'
			       ,'PiMinIPChi2'
			       ,'Pi0MinPT_M'
			       ,'Pi0MinPT_R'
			       ,'ResPi0MinMM'
			       ,'ResPi0MaxMM'
			       ,'ResPi0MinGamCL'
			       ,'BMinM'
			       ,'BMaxM'
			       ,'BMinPT_M'
			       ,'BMinPT_R'
			       ,'BMinVtxProb'
			       ,'BMaxIPChi2'
			       ,'BMinDIRA'
			       ,'BMinVVDChi2'
			       ,'MergedLinePrescale'
			       ,'MergedLinePostscale'
			       ,'ResolvedLinePrescale'
			       ,'ResolvedLinePostscale'
			       ,'prefix'
                               )

##############################################################
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        myPions       = StdNoPIDsPions
	myMergedPi0   = StdLooseMergedPi0
	myResolvedPi0 = StdLooseResolvedPi0
        #---------------------------------------
        # hh selection
        self.selrho = makeB2HHPi0rho( 'B2HHPi0_rho',
                                      config,
                                      DecayDescriptor = 'rho(770)0 -> pi+ pi-',
				      inputSel = [myPions]
                                      )
        #---------------------------------------
        # B -> HHPi0 selections
	self.selresolved = makeB2HHPi0R( 'B2HHPi0R',
	                                 config,
					 DecayDescriptor = 'B0 -> rho(770)0 pi0',
					 inputSel = [self.selrho, myResolvedPi0]
                                         )				       
	self.selmerged = makeB2HHPi0M( 'B2HHPi0M',
	                               config,
				       DecayDescriptor = 'B0 -> rho(770)0 pi0',
				       inputSel = [self.selrho, myMergedPi0]
                                       )
        #---------------------------------------
        # Stripping lines
        self.B2HHPi0R_line = StrippingLine("%(prefix)s_R" %locals()['config'],
                                           prescale = config['ResolvedLinePrescale'],
                                           postscale = config['ResolvedLinePostscale'],
                                           selection = self.selresolved
                                           )
        self.B2HHPi0M_line = StrippingLine("%(prefix)s_M" %locals()['config'],
                                           prescale = config['MergedLinePrescale'],
                                           postscale = config['MergedLinePostscale'],
                                           selection = self.selmerged
                                           )
        # register lines
        self.registerLine(self.B2HHPi0R_line)
        self.registerLine(self.B2HHPi0M_line)

##############################################################
def makeB2HHPi0rho(name,
                   config,
                   DecayDescriptor,
                   inputSel
                   ) :

    _piCuts ="(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" %locals()['config']
    _daughterCuts = { 'pi+' : _piCuts, 'pi-' : _piCuts }
    _combCuts = "(AALL)"  % locals()['config']
    _motherCuts = "(VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVVDCHI2>%(BMinVVDChi2)s)" % locals()['config']

    _rho = CombineParticles( DecayDescriptor = DecayDescriptor,
                             MotherCut = _motherCuts,
                             CombinationCut = _combCuts,
                             DaughtersCuts = _daughterCuts
                             )

    return Selection ( name+'Sel',
                       Algorithm = _rho,
                       RequiredSelections = inputSel )
##############################################################
def makeB2HHPi0R( name,
                  config,
                  DecayDescriptor,
                  inputSel
                  ) :

    _rhoCuts = "(ALL)" % locals()['config']
    _pi0Cuts = "(PT>%(Pi0MinPT_R)s *MeV) & (MM>%(ResPi0MinMM)s *MeV) & (MM<%(ResPi0MaxMM)s *MeV) & (CHILD(CL,1)>%(ResPi0MinGamCL)s) & (CHILD(CL,2)>%(ResPi0MinGamCL)s)" %locals()['config']
    _daughterCuts = { 'rho(770)0' : _rhoCuts, 'pi0' : _pi0Cuts }
    _combCuts = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % locals()['config']
    _motherCuts = "(PT>%(BMinPT_R)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s)" % locals()['config']

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts
                           )

    return Selection( name+'Sel',
                      Algorithm = _B,
                      RequiredSelections = inputSel
                      )
##############################################################
def makeB2HHPi0M( name,
                  config,
                  DecayDescriptor,
                  inputSel
                  ) :

    _rhoCuts = "(ALL)" % locals()['config']
    _pi0Cuts = "(PT>%(Pi0MinPT_M)s *MeV)" %locals()['config']
    _daughterCuts = { 'rho(770)0' : _rhoCuts, 'pi0' : _pi0Cuts }
    _combCuts = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % locals()['config']
    _motherCuts = "(PT>%(BMinPT_M)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s)" % locals()['config']

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts
                           )

    return Selection( name+'Sel',
                      Algorithm = _B,
                      RequiredSelections = inputSel
                      )
##############################################################
