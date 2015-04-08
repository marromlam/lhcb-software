from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
class Run1FilterEvent(Hlt2VoidFilter):
    """
    Filter for the topo events.
    """
    def __init__(self):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules = Factory().Modules
        for module in ['LoKiTrigger.decorators']: 
            if module not in modules: modules.append(module)
        from Inputs import Hlt2BiKalmanFittedForwardTracking
        inputs = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        ec = "CONTAINS('%s') < %%(GEC_MAX)s" % inputs.outputSelection()
        Hlt2VoidFilter.__init__(self, 'TopoEvent', ec, [inputs],
                                shared = True)

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class Run1FilterParts(Hlt2ParticleFilter):
    """
    Filter for the topo input particles.
    """
    def __init__(self, pid, tag, inputs, gec):
        if pid == 'Kaon':
            pc = ("(PT > %%(ALL_PT_MIN)s) & (P > %%(ALL_P_MIN)s) "
                  "& (MIPCHI2DV(PRIMARY) > %%(ALL_MIPCHI2DV_MIN)s) "
                  "& ((TRCHI2DOF < %%(HAD_TRCHI2DOF_MAX)s) "
                  "| ((TRCHI2DOF < %%(%s_TRCHI2DOF_MAX)s)" % tag)
            if   tag == 'MU': pc += " & (HASMUON & ISMUON)))"
            elif tag == 'E':  pc += " & (PIDe > %(PIDE_MIN)s)))"
            else:             pc += '))'
        else:
            pc = ("(BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(MIN_V0_LT)s)")
        from HltTracking.HltPVs import PV3D
        deps = [Run1FilterEvent(), PV3D('Hlt2')] if gec else [PV3D('Hlt2')]
        Hlt2ParticleFilter.__init__(self, 'TopoInput' + pid + tag, pc,
                                    inputs, shared = True, dependencies = deps)

class Run1FilterMforN(Hlt2ParticleFilter):
    """
    Filter m-body combos for an n-body line.
    """
    def __init__(self, tag, m, n, inputs):
        from GaudiKernel.SystemOfUnits import MeV
        if m == 2: pc = ("(M < %f) & (VFASPF(VCHI2) < %%(V2BODYCHI2_MAX)s)" %
                         (5000 * MeV if n == 4 else 6000 * MeV))
        else:      pc = '(M < %f)' % (6000 * MeV)
        Hlt2ParticleFilter.__init__(self, 'Topo%s%iFor%i' % (tag, m, n),
                                    pc, inputs, shared = True)

class Run1FilterNforN(Hlt2ParticleFilter):
    """
    Filter n-body combos for an n-body line.
    """
    def __init__(self, tag, n, inputs):
        from GaudiKernel.SystemOfUnits import MeV
        pids = "(('K+' == ABSID) | ('KS0' == ID) | ('Lambda0' == ABSID))"
        pt = 4000.0 * MeV if n > 2 else 3000.0 * MeV
        pc = ("(SUMTREE(PT, %s, 0.0) > %f) & (NINTREE(('KS0' == ID) "
              "| ('Lambda0' == ABSID)) <= %%(NV0_%iBody_MAX)s) "
              "& (INTREE(ISBASIC "
              "& (MIPCHI2DV(PRIMARY) > %%(ONETRACK_IPCHI2_MIN)s) "
              "& ((PT > %%(ONETRACK_PT_MIN)s) | ((HASMUON & ISMUON) "
              "& (PT > %%(MU_ONETRACK_PT_MIN)s))))) & (MINTREE(HASTRACK "
              "& %s, TRCHI2DOF) < %%(MIN_TRCHI2DOF_MAX)s)"
              % (pids, pt, n, pids))
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'Topo%s%ifor%i' % (tag, n, n),
                                    pc, inputs, shared = True, 
                                    tistos = 'TisTosSpec',
                                    dependencies = [PV3D('Hlt2')])

def Run1PrepVarMap(inputs, varmap):
    """
    Format the variable map for the BBDecTreeTool.
    """
    from copy import deepcopy
    varmap = deepcopy(varmap)
    pids   = "(ABSID == '%s')" % inputs[0]
    for pid in inputs[1:]: pids += " | (ABSID == '%s')" % pid
    ptsum = "SUMTREE(PT, %s, 0.0)/MeV" % pids
    ptmin = "MINTREE(%s, PT)/MeV" % pids
    varmap["PTSUM"] = ptsum
    varmap["PTMIN"] = ptmin
    return varmap

class Run1FilterSimple(Hlt2ParticleFilter):
    """
    Filter for the simple lines.
    """
    def __init__(self, n, inputs, varmap, props):
        from Configurables import BBDTSimpleTool
        from Configurables import LoKi__Hybrid__DictOfFunctors
        from HltLine.HltLine import Hlt1Tool
        varmap  = Run1PrepVarMap(['K+'], varmap)
        handler = Hlt1Tool(type = LoKi__Hybrid__DictOfFunctors, 
                           name = 'VarHandler', Variables = varmap)
        bdttool = Hlt1Tool(type = BBDTSimpleTool, name = 'TrgSimple',
                           Cuts = props['SIMPLE_%dBODY_CUTS' % n], 
                           ParticleDictTool = 'LoKi::Hybrid::DictOfFunctors/' 
                           + handler.Name, tools = [handler])
        pc = "FILTER('BBDTSimpleTool/TrgSimple')"
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, '', pc, inputs, shared = False, 
                                    tools = [bdttool],
                                    dependencies = [PV3D('Hlt2')])

class Run1FilterBDT(Hlt2ParticleFilter):
    """
    Filter for the BDT lines.
    """
    def __init__(self, tag, n, inputs, varmap, props):
        pids = ['K+', 'KS0', 'Lambda0', 'Lambda~0']
        if tag == 'RAD' and n == 3: pids.append('gamma')
        varmap  = Run1PrepVarMap(pids, varmap)
        params  = ('$PARAMFILESROOT/data/Hlt2Topo%dBody_BDTParams_%s.txt' % 
                   (n, props['BDT_%iBODY_PARAMS' % (2 if tag == 'RAD' else n)]))
        bdttool = self.__classifier(params, varmap, "TrgBBDT")
        pc = ("(VALUE('%s/%s') > %%(BDT_%iBODY%s_MIN)s)" % 
              (bdttool.Type.getType(), bdttool.Name, n, tag))
        if   tag == 'MU': pc += " & INTREE(HASPROTO & HASMUON & ISMUON)"
        elif tag == 'E':  pc += " & INTREE(HASPROTO & (PIDe > %(PIDE_MIN)s))"
        from HltTracking.HltPVs import PV3D
        if tag == 'RAD' and props['RAD_TOS']:
            Hlt2ParticleFilter.__init__(self, 'BDT', pc, inputs,
                                        tistos = 'TisTosSpec',
                                        shared = False, tools = [bdttool],
                                        dependencies = [PV3D('Hlt2')])
        else:
            Hlt2ParticleFilter.__init__(self, 'BDT', pc, inputs,
                                        shared = False, tools = [bdttool],
                                        dependencies = [PV3D('Hlt2')])

    def __classifier(self, params, varmap, name, preambulo = []):
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors as DictOfFunctors
        from Configurables import LoKi__Hybrid__DictValue as DictValue
        from Configurables import \
            LoKi__Hybrid__DictTransform_BBDecTreeTransform_ as Transform
        key        = 'BDT'
        options    = {'BBDecTreeFile': params, 'Name': key, 'KeepVars': '0'}
        funcdict   = Tool(type = DictOfFunctors, name = 'TopoMVAdict',
                        Preambulo = preambulo, Variables = varmap)
        transform  = Tool(type = Transform, name = 'BBDecTree',
                          tools = [funcdict], Options = options, 
                          Source = "LoKi::Hybrid::DictOfFunctors/TopoMVAdict")
        classifier = Tool(type = DictValue, name = name, tools = [transform],
                          Key = key, Source = 'LoKi::Hybrid::DictTransform'
                          '<BBDecTreeTransform>/BBDecTree')
        return classifier

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class Run1CombineN(Hlt2Combiner):
    """
    Combiner for all n-body combos.
    """
    def __init__(self, tag, n, inputs):
        if n == 3: decays = [
            "D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-",
            "D*(2010)+ -> K*(892)0 KS0", "D*(2010)+ -> K*(892)0 KS0",
            "D*(2010)+ -> K*(892)0 Lambda0", "D*(2010)+ -> K*(892)0 Lambda~0"]
        elif n == 4: decays = [
            "B0 -> D*(2010)+ K-", "B0 -> D*(2010)+ K+", "B0 -> D*(2010)+ KS0",
            "B0 -> D*(2010)+ Lambda0", "B0 -> D*(2010)+ Lambda~0"]
        else: decays = [
            "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-",
            "K*(892)0 -> K+ KS0", "K*(892)0 -> K- KS0",
            "K*(892)0 -> K+ Lambda0", "K*(892)0 -> K- Lambda0",
            "K*(892)0 -> K+ Lambda~0", "K*(892)0 -> K- Lambda~0",
            "K*(892)0 -> KS0 KS0"]
        cc = ("(AM < 7000*MeV) & ((ANUM((ID=='KS0') | (ABSID=='Lambda0')) > 0) "
              "| ((AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "& (AMAXDOCA('LoKi::DistanceCalculator') < %(AMAXDOCA_MAX)s)))")
        mc = "(BPVDIRA > 0) & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s)"
        if tag == 'RAD': decays = ['B+ -> K*(892)0  gamma']; cc = "AM > 0"
        from HltTracking.HltPVs import PV3D
        dep = [PV3D('Hlt2')]
        if tag == 'RAD':
            from HltLine.HltDecodeRaw import DecodeL0CALO
            dep.append(DecodeL0CALO)
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = Hlt2MergedStage('Topo%s%iBody' % (tag, n), inputs, 
                                 shared = True)
        Hlt2Combiner.__init__(self, 'Topo%s%iBody' % (tag, n), decays,
                              [merged], shared = True, dependencies = dep,
                              CombinationCut = cc, MotherCut = mc)

################################################################################

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class FilterEvent(Hlt2VoidFilter):
    """
    Filter for the topo events.
    """
    def __init__(self):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules = Factory().Modules
        for module in ['LoKiTrigger.decorators']: 
            if module not in modules: modules.append(module)
        from Inputs import Hlt2BiKalmanFittedForwardTracking
        inputs = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        ec = "CONTAINS('%s') < %%(OPT_GEC_MAX)s" % inputs.outputSelection()
        Hlt2VoidFilter.__init__(self, 'OptTopoEvent', ec, [inputs],
                                shared = True)

class FilterParts(Hlt2ParticleFilter):
    """
    Filter for the topo input particles.
    """
    def __init__(self, pid, inputs, gec):
        #pc = ("(PT > %(OPT_TRK_PT_MIN)s) & (P > %(OPT_TRK_P_MIN)s) "
        #      "& (TRCHI2DOF < %(OPT_TRK_CHI2_MAX)s) "
        #      "& (MIPCHI2DV(PRIMARY) > %(OPT_TRK_IPCHI2_MIN)s)")
        #if pid != 'Kaon': pc = ("(BPVLTIME('') > %(OPT_HYP_LT_MIN)s)")
        pc = "ALL"
        from HltTracking.HltPVs import PV3D
        deps = [Run1FilterEvent(), PV3D('Hlt2')] if gec else [PV3D('Hlt2')]
        Hlt2ParticleFilter.__init__(self, 'OptTopoInput' + pid, pc, inputs,
                                    shared = True, dependencies = deps)

class FilterCombos(Hlt2ParticleFilter):
    """
    Filter for the BDT lines.
    """
    def __init__(self, n, inputs, varmap, props):
        #params  = ('$PARAMFILESROOT/data/%s' % props['OPT_BDT_PARAMS'])
        params  = ('$PARAMFILESROOT/data/Hlt2Topo%dBody_BDTParams_%s.txt' % 
                   (n, props['BDT_%iBODY_PARAMS' % n]))
        bdttool = self.__classifier_old(params, varmap, "TrgBBDT")
        pc = ("(VALUE('%s/%s') > %%(OPT_BDT_MIN)s)" % 
              (bdttool.Type.getType(), bdttool.Name))
        #if n == 2:
        #    pc = ("(PT > %(OPT_CMB_PRT_PT_MIN)s) "
        #          "& (VFASPF(VCHI2/VDOF) < %(OPT_CMB_VRT_CHI2_MAX)s) "
        #          "& (BPVVDCHI2 > %(OPT_CMB_VRT_VDCHI2_MIN)s) "
        #          "& (in_range(%(OPT_CMB_VRT_ETA_MIN)s, BPVETA,"
        #          " %(OPT_CMB_VRT_ETA_MAX)s)) "
        #          "& (in_range(%(OPT_CMB_VRT_MCOR_MIN)s, BPVCORRM,"
        #          " %(OPT_CMB_VRT_MCOR_MAX)s)) "
        #          "& (BPVDIRA > %(OPT_CMB_VRT_DIRA_MIN)s) "
        #          "& (NINTREE(MIPCHI2DV(PRIMARY) < 16) <"
        #          " %(OPT_CMB_TRK_NLT16_MAX)s) "
        #          "& " + pc)
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'BDT', pc, inputs, shared = False, 
                                    tools = [bdttool], 
                                    dependencies = [PV3D('Hlt2')])

    def __classifier(self, params, varmap, name, preambulo = []):
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors as DictOfFunctors
        from Configurables import LoKi__Hybrid__DictValue as DictValue
        from Configurables import \
            LoKi__Hybrid__DictTransform_MatrixnetTransform_ as Transform
        key        = 'BDT'
        options    = {'MatrixnetFile': params, 'Name': key, 'KeepVars': '0'}
        funcdict   = Tool(type = DictOfFunctors, name = 'TopoMVAdict',
                        Preambulo = preambulo, Variables = varmap)
        transform  = Tool(type = Transform, name = 'Matrixnet',
                          tools = [funcdict], Options = options, 
                          Source = "LoKi::Hybrid::DictOfFunctors/TopoMVAdict")
        classifier = Tool(type = DictValue, name = name, tools = [transform],
                          Key = key, Source = 'LoKi::Hybrid::DictTransform'
                          '<MatrixnetTransform>/Matrixnet')
        return classifier

    def __classifier_old(self, params, varmap, name, preambulo = []):
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors as DictOfFunctors
        from Configurables import LoKi__Hybrid__DictValue as DictValue
        from Configurables import \
            LoKi__Hybrid__DictTransform_BBDecTreeTransform_ as Transform
        key        = 'BDT'
        options    = {'BBDecTreeFile': params, 'Name': key, 'KeepVars': '0'}
        funcdict   = Tool(type = DictOfFunctors, name = 'TopoMVAdict',
                        Preambulo = preambulo, Variables = varmap)
        transform  = Tool(type = Transform, name = 'BBDecTree',
                          tools = [funcdict], Options = options, 
                          Source = "LoKi::Hybrid::DictOfFunctors/TopoMVAdict")
        classifier = Tool(type = DictValue, name = name, tools = [transform],
                          Key = key, Source = 'LoKi::Hybrid::DictTransform'
                          '<BBDecTreeTransform>/BBDecTree')
        return classifier


from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class CombineTos(Hlt2Combiner):
    """
    Combiner for all 2-body HLT1 TOS combos.
    """
    def __init__(self, inputs):
        pids = ['K+', 'K-', 'KS0', 'Lambda0', 'Lambda~0']
        decays = ['B0 -> %s %s' % (pid1, pid2) for (idx1, pid1)
                  in enumerate(pids) for pid2 in pids[idx1:]]
        #cc = ("(ACUTDOCACHI2(%(OPT_CMB_TRK_DOCA_MAX)s, '')) "
        #      "& (AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16))")
        #mc = ("(HASVERTEX)")
        cc = ("AALL")
        mc = ("ALL")
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('OptTopo2BodyTos', inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'OptTopo2BodyTos', decays,
                              merged, shared = True, #tistos = 'OPT_TOS',
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)

class CombineN(Hlt2Combiner):
    """
    Combiner for all n-body combos.
    """
    def __init__(self, n, inputs):
        pids = ['K+', 'K-', 'KS0', 'Lambda0', 'Lambda~0']
        decays = ['B*0 -> B0 %s %s' % (pid1, pid2) for (idx1, pid1) in
                  enumerate(pids) for pid2 in pids[idx1:]]
        if n == 3: decays = ['B*0 -> B0 %s' % pid for pid in pids]
        #cc = ("(APT > %(OPT_CMB_PRT_PT_MIN)s) "
        #      "& (ANUM(MIPCHI2DV(PRIMARY) < 16) < %(OPT_CMB_TRK_NLT16_MAX)s)")
        #mc = ("(HASVERTEX) "
        #      "& (VFASPF(VCHI2/VDOF) < %(OPT_CMB_VRT_CHI2_MAX)s) "
        #      "& (BPVVDCHI2 > %(OPT_CMB_VRT_VDCHI2_MIN)s) "
        #      "& (in_range(%(OPT_CMB_VRT_ETA_MIN)s, BPVETA,"
        #      " %(OPT_CMB_VRT_ETA_MAX)s)) "
        #      "& (in_range(%(OPT_CMB_VRT_MCOR_MIN)s, BPVCORRM,"
        #      " %(OPT_CMB_VRT_MCOR_MAX)s)) "
        #      "& (BPVDIRA > %(OPT_CMB_VRT_DIRA_MIN)s)")
        cc = ("AALL")
        mc = ("ALL")
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('OptTopo%iBody' % n, inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'OptTopo%iBody' % n, decays, merged,
                              shared = True, dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)
