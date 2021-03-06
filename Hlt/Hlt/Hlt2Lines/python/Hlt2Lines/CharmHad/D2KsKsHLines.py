# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of charm decay channels with
#  two K0s and one hadron in the final state.
#
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of charm decay channels with
    four particles in the final state.
"""
#=============================================================================
__author__  = "Maurizio MARTINELLI maurizio.martinelli@cern.ch"
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm, mrad
import copy

class CharmHadD2KsKsHLines(object):
    def localcuts(self) :
        CommonKsKsH = {
        'KS0LL_ALL_PT_MIN'        : 500.0 * MeV,
        'KS0LL_ALL_MIPCHI2DV_MIN' : 9.0,
        'KS0DD_ALL_PT_MIN'        : 500.0 * MeV,
        'KS0DD_ALL_MIPCHI2DV_MIN' : 9.0,
        'Trk_ALL_PT_MIN'          : 250 * MeV,
        'Trk_ALL_MIPCHI2DV_MIN'   : 3,
        'AM_MIN'                  : 1740. * MeV,
        'AM_MAX'                  : 1990. * MeV,
        'AM_3'                    : 139.57018 * MeV,
        'PT12_MIN'                : 1500 * MeV,
        'ASUMPT_MIN'              : 1500 * MeV,
        'VCHI2PDOF_MAX'           : 20,
        'acosBPVDIRA_MAX'         : 34.6 * mrad,
        'BPVLTIME_MIN'            : 0.4 * picosecond,
        'DPT_MIN'                 : 1800 * MeV,
        'DMOM_MIN'                : 20000 * MeV,
        'TisTosSpec'              : [],#"Hlt1.*Track.*Decision%TOS",
        }

        Dp2KsKsH = copy.deepcopy(CommonKsKsH)
        Dp2KsKsHMass = { 'Mass_M_MIN' : 1765 * MeV,
                         'Mass_M_MAX' : 1965 * MeV }

        Ds2KsKsH = copy.deepcopy(CommonKsKsH)
        Ds2KsKsH.update({   'AM_MIN'                : 1840. * MeV,
                            'AM_MAX'                : 2090. * MeV,
                            'BPVLTIME_MIN'          : 0.2 * picosecond} )
        Ds2KsKsHMass = {    'Mass_M_MIN' : 1865. * MeV,
                            'Mass_M_MAX' : 2065. * MeV }

        Lc2KsKsH = copy.deepcopy(CommonKsKsH)
        Lc2KsKsH.update({   'AM_MIN'                : 2181. * MeV,
                            'AM_MAX'                : 2391. * MeV,
                            'BPVLTIME_MIN'          : 0.1 * picosecond} )
        Lc2KsKsHMass = {    'Mass_M_MIN' : 2156. * MeV,
                            'Mass_M_MAX' : 2366. * MeV }

        cuts = { 'Dp2KsKsH'       : Dp2KsKsH,
                 'Ds2KsKsH'       : Ds2KsKsH,
                 'Lc2KsKsH'       : Lc2KsKsH,
                 'Dp2KsKsHMass'   : Dp2KsKsHMass,
                 'Ds2KsKsHMass'   : Ds2KsKsHMass,
                 'Lc2KsKsHMass'   : Lc2KsKsHMass }

        return cuts

    def locallines(self):
        from Stages import HKsKsCombiner, MassFilter
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import SharedDetachedDpmChild_pi, SharedDetachedDpmChild_K, SharedDetachedLcChild_p

        Dp2KsKsPip_LL =   HKsKsCombiner('Dp2KsKsPip_LL', decay="[D+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_pi],
                           nickname='Dp2KsKsH')
        Dp2KsKsKp_LL =    HKsKsCombiner('Dp2KsKsKp_LL', decay="[D+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_K],
                           nickname='Dp2KsKsH')
        Dsp2KsKsPip_LL =   HKsKsCombiner('Dsp2KsKsPip_LL', decay="[D_s+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_pi],
                           nickname='Ds2KsKsH')
        Dsp2KsKsKp_LL =    HKsKsCombiner('Dsp2KsKsKp_LL', decay="[D_s+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_K],
                           nickname='Ds2KsKsH')
        Lcp2KsKsPp_LL =   HKsKsCombiner('Lcp2KsKsPp_LL', decay="[Lambda_c+ -> KS0 KS0 p+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedLcChild_p],
                           nickname='Lc2KsKsH')


        Dp2KS0KS0Pip = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsPip_LL])
        Dp2KS0KS0Kp = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsKp_LL])
        Dsp2KS0KS0Pip = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsPip_LL])
        Dsp2KS0KS0Kp = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsKp_LL])
        Lcp2KS0KS0Pp = MassFilter('Lc2KsKsHMass',inputs=[Lcp2KsKsPp_LL])

        stages = {
                  'Dp2KS0KS0PipTurbo'   : [Dp2KS0KS0Pip],
                  'Dp2KS0KS0KpTurbo'    : [Dp2KS0KS0Kp],

                  'Dsp2KS0KS0PipTurbo'  : [Dsp2KS0KS0Pip],
                  'Dsp2KS0KS0KpTurbo'   : [Dsp2KS0KS0Kp],

                  'Lcp2KS0KS0PpTurbo'   : [Lcp2KS0KS0Pp],
                 }

        return stages
