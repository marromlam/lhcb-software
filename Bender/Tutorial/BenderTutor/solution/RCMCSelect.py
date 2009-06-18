#!/usr/bin/env python
# =============================================================================
# $Id: RCMCSelect.py,v 1.16 2009-06-18 12:46:20 ibelyaev Exp $
# =============================================================================
"""
'Solution'-file for 'RCMCselect.py' example (Bender Tutorial)
"""
# =============================================================================
## @file
#  "Solution"-file for 'RCMCselect.py' example (Bender Tutorial)
#  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
#  @date   2004-10-12
# =============================================================================
__author__  = ' Vanya BELYAEV  Ivan.Belyaev@nikhef.nl '
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.16 $  '  
# =============================================================================
## import everything from BENDER
from Bender.MainMC import *
# =============================================================================
## @class RCSelect
#  my analysis algorithm 
class RCMCSelect(AlgoMC):
    """
    My analysis algorithm
    """
    
    ## constructor
    def __init__ ( self , name = 'RCMCSelect' , **args ) :
        """
        The constructor
        """
        AlgoMC.__init__ ( self , name )
        for k in args : setattr ( self , k , args[k] )
        

    ## the main analysis method 
    def analyse( self ) :
        """
        The main analysis method
        """
        ## get MCDecayFinder wrapper: 
        finder = self.mcFinder()
        ## find all MC trees  
        mcBs  = finder.find(
            '[ B_s0 -> (  J/psi(1S) -> mu+  mu- {,gamma} )  phi(1020)]cc' )
        if mcBs.empty() : return SUCCESS
        
        ## get all MC phis from the tree :
        mcPhi = finder.find(
            '[ B_s0 -> (  J/psi(1S) -> mu+  mu- {,gamma} ) ^phi(1020)]cc' )
        if mcPhi.empty() : return SUCCESS

        ## get all MC psis from the tree :
        mcPsi = finder.find(
            '[ B_s0 -> ( ^J/psi(1S) -> mu+  mu- {,gamma} )  phi(1020)]cc' )
        if mcPsi.empty() : return SUCCESS

        ## get helper object for MC-match
        match = self.mcTruth( 'MCDecayMatch')
        # prepare "Monte-Carlo Cuts"
                
        mcCutBs  = MCTRUTH ( match , mcBs  )
        mcCutPhi = MCTRUTH ( match , mcPhi )
        mcCutPsi = MCTRUTH ( match , mcPsi )
        
        ## select muons for J/Psi reconstruction 
        muons = self.select( "mu" , ( "mu+" == ABSID ) &  ( PT > 500 ) )
        if muons.empty() : return SUCCESS
        ## select kaons for Phi reconstruction
        kaons = self.select( "K"  , ( "K+" == ABSID ) & ( PIDK > 0.0  ) )
        if kaons.empty() : return SUCCESS
        ## delta mass cut for J/psi  
        dmPsi = ADMASS('J/psi(1S)') < 50
        ## prepare the loop over dimuons 
        psis = self.loop ( 'mu mu' , 'J/psi(1S)' )
        for psi in psis :
            ## use *ONLY* Monte-Carlo cuts
            if not mcCutPsi( psi ) : continue    ## ATTENTION! only true J/psi
            ## rought estimation of the mass 
            mass = psi.mass(1,2) / 1000 
            if not 2.5 <  mass < 3.5 : continue
            ## neutral combination?
            if not 0 == SUMQ( psi )          : continue 
            ## check the chi2 of the vertex fit
            if not 0 <= VCHI2( psi )  < 49   : continue
            self.plot( M(psi) / 1000 ,
                       " dimuon invariant mass " ,
                       2.5 , 3.5 ) 
            if not dmPsi( psi ) : continue
            psi.save( 'psi' )
            
        ## delta mass cut for phi 
        dmPhi = ADMASS('phi(1020)') < 20 
        ## prepare the loop over dikaons             
        phis = self.loop( 'K K' , 'phi(1020)' )
        for phi in phis :
            ## use *ONLY* Monte-Carlo cuts
            if not mcCutPhi( phi ) : continue  ## ATTNETION: only true phi
            #
            if phi.mass( 1 ,2 ) > 1050       : continue 
            # neutral combination ?
            if not 0 == SUMQ( phi )          : continue
            if not 0 <= VCHI2( phi ) < 49    : continue            
            self.plot ( M(phi) / 1000  ,
                        " dikaon invariant mass " ,
                        1.0 , 1.050         )
            if not dmPhi( phi ) : continue
            phi.save('phi')
        ## delta mass cut for Bs 
        dmBs = ADMASS('B_s0') < 100
        ## prepare the loop over psi+phi combinations 
        bs = self.loop( 'psi phi' , 'B_s0' )
        for B in bs :
            ## use *ONLY* Monte-Carlo cuts
            if not mcCutBs( B ) : continue   ## ATTENTION: only true Bs
            #
            m = B.mass(1,2) / 1000 
            if not 4.5 < m           < 6.5  : continue
            if not 0   < VCHI2( B )  < 49   : continue
            self.plot ( M(B) / 1000 ,
                        " psi phi invariant mass " ,
                        5.0  , 6.0        )
            if not dmBs ( B ) : continue
            B.save('Bs')
            
        Bs = self.selected('Bs')
        if not Bs.empty() : self.setFilterPassed ( True ) ## FILTER PASSED
        
        return SUCCESS 
# =============================================================================

# =============================================================================
## Job configuration:
def configure() :
    """
    Job configuration
    """
    
    from Configurables import DaVinci
    
    DaVinci (
        DataType   = 'DC06'     , # default  
        Simulation = True       ,
        HltType    = '' ) 
    
    from Gaudi.Configuration import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'RCMCselect_histos.root' ) 

    ## get/create application manager
    gaudi = appMgr() 
    
    # modify/update the configuration:
    # 1) create the algorithm
    alg = RCMCSelect(
        'RCMCSelect' ,
        InputLocations = [ 'Phys/StdTightKaons' , 
                           'Phys/StdTightMuons' ] ,
        PP2MCs = ['Relations/Rec/ProtoP/Charged']
        )
                      
    # 2) add the algorithm
    #gaudi.addAlgorithm( alg )
    gaudi.setAlgorithms( [alg] )
    
     
    ## redefine input files 
    evtSel = gaudi.evtSel()
    import LoKiExample.Bs2Jpsiphi_mm_data as data 
    evtSel.open( data.Files ) 

    
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering:
if __name__ == '__main__' :

    ## job configuration
    configure()
    ## event loop 
    run(2000)

# =============================================================================
# The END 
# =============================================================================
