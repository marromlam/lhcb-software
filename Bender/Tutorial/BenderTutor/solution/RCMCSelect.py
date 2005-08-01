#!/usr/bin/env python
# =============================================================================
# $Id: RCMCSelect.py,v 1.6 2005-08-01 16:04:24 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
# =============================================================================
""" 'Solution'-file for 'RCMCselect.py' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'RCMCselect.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# the algorithm itself 
# =============================================================================
class RCSelect(Algo):
    def analyse( self ) :

        mc = self.mctruth( name = 'MCdecayMatch')
        
        # find all MC trees  
        mcBs  = mc.find(
            decay = '[ B_s0 -> (  J/psi(1S) -> mu+  mu- )  phi(1020)]cc' )
        
        # get all MC phis from the tree :
        mcPhi = mc.find(
            decay = '[ B_s0 -> (  J/psi(1S) -> mu+  mu- ) ^phi(1020)]cc' )
        
        # get all MC psis from the tree :
        mcPsi = mc.find(
            decay = '[ B_s0 -> ( ^J/psi(1S) -> mu+  mu- )  phi(1020)]cc' )
        
        # prepare "Monte-Carlo Cuts"
        mcCutBs  = MCTRUTH( mc , mcBs )
        mcCutPhi = MCTRUTH( mc , mcPhi )
        mcCutPsi = MCTRUTH( mc , mcPsi )
        
        # select muons for J/Psi reconstruction 
        muons = self.select( tag  = "mu" ,
                             cuts = ( "mu+" == ABSID ) & ( PT > ( 0.5 * GeV ) ) )
        if muons.empty() : return SUCCESS
        
        # select kaons for Phi reconstruction
        kaons = self.select( tag = "K"  ,
                             cuts = ( "K+" == ABSID ) & ( PIDK > 0.0  ) )
        if kaons.empty() : return SUCCESS
        
        # delta mass cut fro J/psi  
        dmPsi = ADMASS('J/psi(1S)') < ( 50 * MeV )
        
        psis = self.loop ( formula = 'mu mu' , pid = 'J/psi(1S)' )
        for psi in psis :
            #
            # use Monte-Carlo cuts
            if not mcCutPsi( psi ) : continue 
            #
            mass = psi.mass(1,2)
            if not 2.5*GeV <  mass < 3.5*GeV : continue
            # neutral combination?
            if not 0 == SUMQ( psi )          : continue 
            # check the chi2 of the vertex fit
            if not 0 <= VCHI2( psi )  < 49   : continue
            
            self.plot( title = " dimuon invariantt mass " ,
                       value = M(psi) / GeV ,
                       low   = 2.5          ,
                       high  = 3.5          ) 
            if not dmPsi( psi ) : continue
            psi.save( 'psi' )
            
        # delta mass cut for phi 
        dmPhi = ADMASS('phi(1020)') < ( 12 * MeV )        
            
        phis = self.loop( formula = 'K K' , pid = 'phi(1020)' )
        for phi in phis :

            # use Monte-Carlo cuts
            if not mcCutPhi( phi ) : continue
            #
            if phi.mass( 1 ,2 ) > 1050 * MeV : continue 
            # neutral combination ?
            if not 0 == SUMQ( phi )          : continue
            if not 0 <= VCHI2( phi ) < 49    : continue
            
            self.plot ( title = " dikaon invarinat mass " ,
                        value = M(phi) / GeV  ,
                        low   = 1.0           ,
                        high  = 1.050         )
            if not dmPhi( phi ) : continue
            phi.save('phi')

        # delta mass cut for Bs 
        dmBs = ADMASS('B_s0') < ( 100 * MeV )      
        bs = self.loop( formula = 'psi phi' , pid = 'B_s0' )
        for B in bs :

            # use Monte-Carlo cuts
            if not mcCutBs( B ) : continue
            #
            if not 4.5*GeV <   B.mass(1,2) < 6.5*GeV  : continue
            if not 0       <=  VCHI2( B )  < 49       : continue
            
            self.plot ( title = " psi phi invarinat mass " ,
                        value = M(B) / GeV ,
                        low   = 5.0        ,
                        high  = 6.0        )
            if not dmBs ( B ) : continue
            B.save('Bs')

        Bs = self.selected('Bs')
        if not Bs.empty() : self.setFilterPassed ( True )
        
        return SUCCESS 
# =============================================================================

# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    gaudi.config ( files =
                   [ '$DAVINCIROOT/options/DaVinciCommon.opts'   ,
                     '$DAVINCIROOT/options/DaVinciReco.opts'     ] )
    
    # modify/update the configuration:
    
    # Preload all charged particles 
    gaudi.addAlgorithm( 'LoKiPreLoad/Charged' ) 
    
    import benderPreLoad as preload
    
    # useful configuration of LoKiPreloadAlgorithm
    preload.Charged ( Name  = 'Charged'                                , 
                      Kaons = [ "det='RICH' k-pi='-5.0' k-p='-5.0'"  ] ,
                      Pions = [ "det='RICH' pi-k='-5.0' pi-p='-5.0'" ] )
    
    # 1) create the algorithm
    alg = RCSelect( 'RCSelect' )
    
    # 2) add the algorithm
    gaudi.addAlgorithm( alg ) 
    
    # 3) configure algorithm
    desktop = gaudi.tool('RCSelect.PhysDesktop')
    desktop.InputLocations = [ "/Event/Phys/Charged" ]
    
    # configure the histograms:
    hsvc = gaudi.histoSvc()
    hsvc.setOutput('myhistos.hbook', 'HBOOK')
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    # suppress printout form DaVinci
    dv = gaudi.algorithm('DaVinci')
    dv.doHistos = False
    
    myAlg = gaudi.algorithm('RCSelect')
    myAlg.MCppAssociators = [
        'AssociatorWeighted<ProtoParticle,MCParticle,double>/ChargedPP2MCAsct' ]
    
    # redefine input files 
    evtsel = gaudi.evtSel()
    evtsel.PrintFreq = 20 
    # Bs -> Kpsi(mu+mu-) phi(K+K-_) data 
    evtsel.open( stream = [
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000017_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000018_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000016_5.dst' , 
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000020_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000024_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000019_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000021_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000022_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000001_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000002_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000003_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000004_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000005_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000006_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000007_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000008_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000009_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000010_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000012_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000013_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000014_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000015_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000023_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000025_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000026_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000027_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000028_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000029_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000030_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000031_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000032_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000033_5.dst' ] )
    
    return SUCCESS
# =============================================================================

# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    gaudi.run(500)

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
