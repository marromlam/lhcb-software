#!/usr/bin/env python
# =============================================================================
# $Id: get_metainfo.py 152803 2013-02-23 13:29:42Z ibelyaev $ 
# =============================================================================
## @file 	
#  Simple script to extract MetaInfo from data files 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
#  @code
# 
#  Usage:
#
#     get_metainfo [options] file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-24
#
#                    $Revision: 152803 $
#  Last modification $Date: 2013-02-23 14:29:42 +0100 (Sat, 23 Feb 2013) $
#                 by $Author: ibelyaev $
#
# =============================================================================
"""
Simple script to extract MetaInfo from data files 

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    get_metainfo [options] file1 [ file2 [ file3 [ file4 ....'

"""
# =============================================================================
__author__  = "Vanya BELYAEV"
__date__    = "2012-10-24"
__version__ = "$Revision:$"

# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print                   __doc__ 
    print ' Author(s)  : ', __author__ 
    print ' Version    : ', __version__
    print ' Date       : ', __date__ 
    print 120*'*'
    
    from BenderTools.GetDBtags  import extractMetaInfo

    import sys
    extractMetaInfo ( sys.argv[1:] )
    
# =============================================================================
# The END 
# =============================================================================
