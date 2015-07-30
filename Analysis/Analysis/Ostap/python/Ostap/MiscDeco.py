#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of Canvas and other objects for efficient use in python
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Decoration of Canvas and other  objects for efficient use in python """
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT, cppyy              ## attention here!!
cpp = cppyy.makeNamespace('')
VE  = cpp.Gaudi.Math.ValueWithError 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.MiscDeco' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'Some useful decorations for Canvas and other objects')
# ==============================================================================


# =============================================================================
## define simplified print for TCanvas 
def _cnv_print_ ( cnv , fname , exts = [ 'pdf' , 'gif' , 'eps', 'C' ] ) :
    """
    A bit simplified version for TCanvas print
    
    >>> canvas.print ( 'fig' )    
    """
    #
    p = fname.rfind ('.')
    #
    if 0 < p :
        if p+4 == len ( fname ) or fname[p:] in ( '.C', '.ps', '.jpeg', '.JPEG') :
            cnv.Print( fname )
            return cnv 
        
    for e in exts :
        cnv.Print ( fname + '.' + e )
            
    return cnv 

# =============================================================================
## define streamer for canvas 
def _cnv_rshift_ ( cnv , fname ) :
    """
    very simple print for canvas:
    
    >>> canvas >> 'a'
    
    """
    return _cnv_print_ ( cnv , fname )

ROOT.TCanvas.print_     = _cnv_print_
ROOT.TCanvas.__rshift__ = _cnv_rshift_



# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
