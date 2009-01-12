# =============================================================================
# $Id: HltCommissioningLines.py,v 1.2 2009-01-12 11:02:19 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for commissioning
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
 script to configure Hlt lines for commissioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

from LHCbKernel.Configuration import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from Configurables       import HltIncidentFilter


class HltCommissioningLinesConf(LHCbConfigurableUser):

   __slots__ = { 'PhysicsPrescale'  : 1
               , 'RandomPrescale'   : 1
               , 'L0ForcedPrescale'   : 1
               }
   def __apply_configuration__(self):
        Line('Physics' ,  ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
            , prescale = self.getProp('PhysicsPrescale') 
            )

        Line('Random' ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'
            , prescale = self.getProp('RandomPrescale') # @OnlineEnv.AcceptRate
            )

        Line('L0Forced', L0DU = 'L0_FORCEBIT'
            , prescale = self.getProp('L0ForcedPrescale')
            )
        Line('Incident', ODIN = 'ODIN_ALL', algos = [ HltIncidentFilter() ])
