#!/usr/bin/env python
# =============================================================================
# $Id: Bs2DsK.py,v 1.3 2009-05-14 17:55:00 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/Bs2DsK.py
#  The simple Bender-based example: find recontructed Bs -> BsK candidates 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The simple Bender-based example: find recontructed Bs -> BsK candidates 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
# =============================================================================
## import everything form bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Bs2DsK(AlgoMC) :
    """
    find recontructed Bs -> Ds K candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'Bs2DsK' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )

    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        # Ds+/Ds- -> K+ K- pi+/pi-
        ds = Trees.MCExclusive  ( Nodes.CC('D_s+') , 1 , True )
        ds += 'K+'
        ds += 'K-'
        ds += Nodes.CC('pi+')
        
        ## Xb -> ^Ds+/Ds- K+/K-
        bs = Trees.MCExclusive ( Nodes.Beauty )
        bs += mark ( ds )     # mark Ds!
        bs += Nodes.CC("K+") 
        
        st =  bs.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        cut = MCDECTREE(bs) 
        mcbs = self.mcselect ('mcbs' , cut )
        if mcbs.empty() or 1 < mcbs.size() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        # collect "marked" paricles
        mcds = std.vector('const LHCb::MCParticle*')()

        bs.reset() 
        for b in mcbs :
            if bs ( b ) :
                bs.collect ( mcds )
                bs.reset()
                
        mcBs = MCTRUTH ( self.mcTruth() , mcbs )        
        mcDs = MCTRUTH ( self.mcTruth() , mcds )
        
        kaons = self.select ( 'kaons' ,
                              ( 'K+' == ABSID ) &
                              ( PIDK > 2      ) & mcDs )
        pions = self.select ( 'pion'   ,
                              ( 'pi+' == ABSID )  &
                              ( PIDK  < 1       ) &
                              ( PIDe  < 1       ) &
                              ( PIDmu < 1       ) & mcDs )
        
        bkaons = self.select ( 'bk' ,
                               ( 'K+' == ABSID ) & mcBs & ~mcDs )
        
        k1p = self.select ( 'k+' , kaons , 0 < Q )
        k1m = self.select ( 'k-' , kaons , 0 > Q )
        
        ds = self.loop ( 'k+ k- pion' , 'D_s+' )
        for D in ds :
            m123 = D.mass(1,2,3) / Units.GeV
            if not 1.4 < m123 < 2.0 : continue
            chi2 = VCHI2 ( D )
            if not 0 <=  chi2 < 100 : continue
            if  not mcDs ( D )      : continue 
            self.plot ( M(D) / Units.GeV , 'mass K+ K- pi' , 1.8 , 2.1 , 300 )
            sq = SUMQ( D )
            if sq < 0 : D.setPID ( 'D_s-' )
            D.save('Ds')
            
        bs = self.loop ( 'Ds bk' , 'B_s0')
        for B in bs :
            m12 = B.mass(1,2) / Units.GeV
            if not 3 < m12  < 7   : continue
            chi2 = VCHI2 ( B ) 
            if not 0<= chi2 < 100 : continue 
            if not mcDs ( B(1) )  : continue
            if not mcBs ( B(2) )  : continue
            if not mcBs ( B    )  : continue 
            self.plot ( M(B) / Units.GeV , 'mass Ds K' , 4.0 , 6.0 , 200 )
            B.save ('Bs') 

        Bs = self.selected('Bs')
        self.setFilterPassed ( not Bs.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        DataType   = 'DC06'  , 
        Simulation = True    ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'Bs2DsK_Histos.root' ) 
    
    EventSelector (
##         ##-- GAUDI data cards generated on 12/22/08 2:34 PM
##         ##-- For Event Type = 13264011 / Data type = DST 1
##         ##--     Configuration = DC06 - phys-v2-lumi2
##         ##--     DST 1 datasets produced by Brunel - v30r17
##         ##--     From DIGI 1 datasets produced by Boole - v12r10
##         ##--     From SIM 1 datasets produced by Gauss - v25r12
##         ##--     Database version = v30r14
##         ##--     Cards content = physical-physical
##         ##--     
##         ##-- Datasets replicated at CERN
##         ##-- 50 dataset(s) - NbEvents = 24847
##         ##-- 
##         Input   = [
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000040_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000043_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000046_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000047_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000048_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000050_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000051_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000052_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000055_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000056_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000058_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000059_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000060_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000061_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000062_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000063_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000064_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
##         ]
        Input = [
        # Bs -> Ds K 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_3.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_4.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         # Bs -> Ds pi 
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_3.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         ,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_4.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
    )
    
    gaudi = appMgr() 
    
    ## create the local algorithm:
    alg = Bs2DsK()
    
    ## gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
    ## PP -> MC relation tables 
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    
    ## print histos 
    alg.HistoPrint = True
    
    ## define the input locations 
    alg.InputLocations = [
        '/Event/Phys/StdTightKaons' ,
        '/Event/Phys/StdTightPions' ,
        '/Event/Phys/StdNoPIDsKaons' 
        ]
    
    evtSel = gaudi.evtSel()    
    evtSel.PrintFreq = 50

    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__

    ## configure the job:
    configure()

    ## run the job
    run(1000)
    

# =============================================================================
# The END 
# =============================================================================
