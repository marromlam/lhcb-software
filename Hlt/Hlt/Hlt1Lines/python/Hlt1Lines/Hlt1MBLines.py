##############################
#     HLT MiniBias Line(s)
##############################

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line as Line

############# start building the MB  line(s)...
class Hlt1MBLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'MiniBiasL0Channels'     : ['CALO'] #'Hadron'
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                , 'Prescale'               : { 'Hlt1MBNoBias' : 'RATE(100)' }
                }

    def __create_nobias_line__(self ):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        return Line ( 'MBNoBias'
                    , prescale = self.prescale
                    , ODIN = '(ODIN_TRGTYP == LHCb.ODIN.LumiTrigger)'
                    , postscale = self.postscale
                    ) 
    def __create_microbias_line__(self, name, tracking) :
        from HltLine.HltLine import Hlt1Member as Member
        Line ( 'MBMicroBias%s' % name 
               , prescale = self.prescale
               , ODIN = '(ODIN_TRGTYP == LHCb.ODIN.LumiTrigger)'
               , algos = [ tracking
                           , Member( 'Hlt::TrackFilter','All'
                                     , Code = [ 'TrALL' ]
                                     , InputSelection = 'TES:%s' % tracking.outputSelection()
                                     , OutputSelection = '%Decision'
                                     ) 
                           ]
               , postscale = self.postscale
               ) 

    def __create_minibias_line__(self ):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        return Line ( 'MBMiniBias'
                    , prescale = self.prescale
                    , L0DU  = ' | '.join( [ "L0_CHANNEL('%s') "%(x) for x in  self.getProp('MiniBiasL0Channels') ] )
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        self.__create_minibias_line__()
        self.__create_nobias_line__()

        from HltLine.HltReco import MinimalRZVelo, Hlt1Seeding
        self.__create_microbias_line__('RZVelo',MinimalRZVelo)
        self.__create_microbias_line__('TStation',Hlt1Seeding)
