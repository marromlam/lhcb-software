import importlib
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class TurboLines(Hlt2LinesConfigurableUser) :
    __slots__ = {'Prescale' : {},
                 'Postscale' : {},
                 'CloneLines'  : {'DiMuon' : ['JPsi', 
                                              'Psi2S', 
                                              'B'],
			          'TrackEffDiMuon': ['MuonTTPlusTagged',
					             'MuonTTMinusTagged',
					             'VeloMuonPlusTagged',
					             'VeloMuonMinusTagged',
					             'DownstreamPlusTagged',
					             'DownstreamMinusTagged',
					             'MuonTTPlusMatched',
					             'MuonTTMinusMatched',
					             'VeloMuonPlusMatched',
					             'VeloMuonMinusMatched',
					             'DownstreamPlusMatched',
					             'DownstreamMinusMatched'
					            ]
			         }
                 }

    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        lines = self.getProp('CloneLines')
        for (module, lines) in lines.iteritems():
            # Import the other module that has the stages.
            mod = importlib.import_module('Hlt2Lines.' + module + '.Lines')
            # Instantiate the configurable
            conf = getattr(mod, module + 'Lines')()
            # Loop over the line, use the configurable to get the algorithms and
            # instantiate the turbo lines.
            for line in lines:
                stages = conf.stages(line)
                algos = conf.algorithms({'Turbo' : stages})
                Hlt2Line('New' + module + line + 'Turbo', prescale = self.prescale,
                         algos = algos['Turbo'], postscale = self.postscale, Turbo = True)
                if module is 'DiMuon':
                    if line is 'JPsi':
                        HltANNSvc().Hlt2SelectionID.update( { 'Hlt2New'+module+line+'Turbo'+'Decision':  50212 } ) 
