#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file FitOtherModels.py
#
#  A set of variosu smooth shapes and PDFs 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
# 
#                    $Revision: 172520 $
#  Last modification $Date: 2014-05-10 20:27:44 +0200 (Sat, 10 May 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
"""
A set of various smooth shapes and PDFs 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    'GammaDist_pdf'      , ## Gamma-distributuon in shape/scale parameterization
    'GenGammaDist_pdf'   , ## Generalized Gamma-distribution
    'Amoroso_pdf'        , ## another view of generalized Gamma distribution
    'LogGammaDist_pdf'   , ## Gamma-distributuon in shape/scale parameterization
    'Log10GammaDist_pdf' , ## Gamma-distributuon in shape/scale parameterization
    'LogGamma_pdf'       , ## 
    'BetaPrime_pdf'      , ## Beta-prime distribution 
    )
# =============================================================================
import ROOT, math
from   PyPAW.PyRoUts             import VE,cpp
from   GaudiKernel.SystemOfUnits import GeV 
# =============================================================================
from   AnalysisPython.Logger     import getLogger
logger = getLogger ( __name__ ) 
# =============================================================================
# Specializations of double-sided Crystal Ball function 
# =============================================================================
from   PyPAW.FitBasic             import makeVar
# =============================================================================
## @class GammaDist_pdf
#  Gamma-distribution with shape/scale parameters
#  http://en.wikipedia.org/wiki/Gamma_distribution
#  It suits nicely for fits of multiplicity and/or chi2 distributions
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::GammaDist 
#  @see Gaudi::Math::GammaDist 
class GammaDist_pdf(object) :
    """
    Gamma-distribution with shape/scale parameters
    http://en.wikipedia.org/wiki/Gamma_distribution
    It suits nicely for firs of multiplicity and/or chi2 distributions
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   k     = None     ,   ## k-parameter
                   theta = None     ) : ## theta-parameter
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'kGamma_%s'           % name ,
                               'k_{#Gamma}(%s)'      % name , None , 1 , 1.e-3 , 100 )
        self.theta = makeVar ( theta   ,
                               'tGamma_%s'           % name ,
                               '#theta_{#Gamma}(%s)' % name , None , 1 , 1.e-3 , 100 )
        
        if self.k.getMin() <= 0 :
            self.k.setMin ( 1.e-3 ) 
            logger.warning( 'GammaDist_pdf: k min is set to be %s ' % self.k.getMin() ) 
            
        if self.theta.getMin() <= 0 :
            theta.setMin ( 1.e-3 ) 
            logger.warning( 'GammaDist_pdf: theta min is set to be %s ' % self.theta.getMin() ) 
            
        self.pdf  = cpp.Analysis.Models.GammaDist (
            'gd_%s'         % name ,
            'GammaDist(%s)' % name ,
            self.x                 ,
            self.k                 ,
            self.theta             )

# =============================================================================
## @class GenGammaDist_pdf 
#  Generalized Gamma-distribution with additional shift parameter 
#  http://en.wikipedia.org/wiki/Generalized_gamma_distribution
#  Special cases : 
#  - p == 1      : Gamma  distribution
#  - p == k      : Weibull distribution
#  - p == k == 1 : Exponential distribution
#  - p == k == 2 : Rayleigh    distribution
#  @see Gaudi::Math::GenGammaDist
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Gaudi::Math::GenGammaDist 
#  @see Analysis::Models::GenGammaDist 
class GenGammaDist_pdf(object) :
    """
    Generalized Gamma-distribution with additional shift parameter 
    http://en.wikipedia.org/wiki/Generalized_gamma_distribution
    Special cases : 
    - p == 1      : Gamma  distribution
    - p == k      : Weibull distribution
    - p == k == 1 : Exponential distribution
    - p == k == 2 : Rayleigh    distribution
    """
    ## constructor
    def __init__ ( self          ,
                   name          ,   ## the name 
                   x             ,   ## the variable
                   k     = None  ,   ## k-parameter
                   theta = None  ,   ## theta-parameter
                   p     = None  ,   ## p-parameter
                   low   = None  ) : ## low-parameter
        #
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'kGamma_%s'           % name ,
                               'k_{#Gamma}(%s)'      % name , None , 1 , 1.e-3 , 100 )
        self.theta = makeVar ( theta   ,
                               'tGamma_%s'           % name ,
                               '#theta_{#Gamma}(%s)' % name , None , 1 , 1.e-3 , 100 )
        self.p     = makeVar ( p       ,
                               'pGamma_%s'           % name ,
                               'p_{#Gamma}(%s)'      % name , None , 1 , 1.e-3 ,  20 )
        
        self.low   = makeVar ( low      ,
                               'lGamma_%s'           % name ,
                               'l_{#Gamma}(%s)'      % name ,
                               None , min ( 0 , x.getMin() ) , x.getMax() ) 
        
        if self.k    .getMin() <= 0 :
            self.k   .setMin ( 1.e-3 ) 
            logger.warning( 'GenGammaDist_pdf: min(k)     is set %s ' % self.k.getMin() ) 
            
        if self.theta.getMin() <= 0 :
            self.theta.setMin ( 1.e-3 ) 
            logger.warning( 'GenGammaDist_pdf: min(theta) is set %s ' % self.theta.getMin() )
            
        if self.p    .getMin() <= 0 :
            self.p   .setMin ( 1.e-3 ) 
            logger.warning( 'GenGammaDist_pdf: min(p)     is set %s ' % self.p.getMin()    )
            
        self.pdf  = cpp.Analysis.Models.GenGammaDist (
            'ggd_%s'           % name ,
            'GenGammaDist(%s)' % name ,
            self.x         ,
            self.k         ,
            self.theta     ,
            self.p         , 
            self.low       )


# =============================================================================
## @class Amoroso_pdf
#  Another view on generalized gamma distribution
#  http://arxiv.org/pdf/1005.3274
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Gaudi::Math::Amoroso
#  @see Analysis::Models::Amoroso
class Amoroso_pdf(object) :
    """
    Another view on generalized gamma distribution
    http://arxiv.org/pdf/1005.3274
    """
    ## constructor
    def __init__ ( self          ,
                   name          ,   ## the name 
                   x             ,   ## the variable
                   theta = None  ,   ## theta-parameter
                   alpha = None  ,   ## alpha-parameter
                   beta  = None  ,   ## beta-parameter
                   a     = None  ) : ## s-parameter
        
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.theta = makeVar ( theta   ,
                               'tAmoroso_%s'          % name ,
                               '#theta_{Amoroso}(%s)' % name , None , 1 , 1.e-3 , 100 )
        self.alpha = makeVar ( alpha   ,
                               'alpha_Amoroso_%s'     % name ,
                               '#alpha_{Amoroso}(%s)' % name , None , 1 , 1.e-3 , 100 )
        self.beta  = makeVar ( beta    ,
                               'beta_Amoroso_%s'      % name ,
                               '#beta_{Amoroso}(%s) ' % name , None , 1 , 1.e-3 ,  10 )        
        self.a     = makeVar ( a       ,
                               'a_Amoroso_%s'         % name ,
                               'a_{Amoroso}(%s)'      % name , None , 1 , -10   ,  10  )
            
        self.pdf  = cpp.Analysis.Models.Amoroso (
            'amo_%s'      % name ,
            'Amoroso(%s)' % name ,
            self.x         ,
            self.theta     ,
            self.alpha     ,
            self.beta      ,
            self.a         )

# =============================================================================
## @class LogGammaDist_pdf
#  Distribution for log(x), where x follows Gamma distribution
#  It suits nicely for fits of log(multilicity) and/or log(chi2) distributions
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::LogGammaDist 
#  @see Gaudi::Math::LogGammaDist 
#  @see AnalysisModels::GammaDist 
#  @see Gaudi::Math::GammaDist 
class LogGammaDist_pdf(object) :
    """
    Gamma-distribution for log(x), where x follows Gamma distribution
    It suits nicely for fits of log(multilicity) and/or log(chi2) distributions
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   k     = None     ,   ## k-parameter
                   theta = None     ) : ## theta-parameter
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'kGamma_%s'           % name ,
                               'k_{#Gamma}(%s)'      % name , None , 1 , 1.e-3 , 100 )
        self.theta = makeVar ( theta   ,
                               'tGamma_%s'           % name ,
                               '#theta_{#Gamma}(%s)' % name , None , 1 , 1.e-3 , 100 )
        
        if self.k.getMin() <= 0 :
            self.k.setMin ( 1.e-3 ) 
            logger.warning( 'LogGammaDist_pdf: min(k)     is set %s ' % self.k.getMin() ) 
            
            if self.theta.getMin() <= 0 :
                theta.setMin ( 1.e-3 ) 
            logger.warning( 'LogGammaDist_pdf: min(theta) is set %s ' % self.theta.getMin() ) 
            
        self.pdf  = cpp.Analysis.Models.LogGammaDist (
            'lgd_%s'           % name ,
            'LogGammaDist(%s)' % name ,
            self.x                 ,
            self.k                 ,
            self.theta             )


# =============================================================================
## @class Log10GammaDist_pdf
#  Distribution for log10(x), where x follows Gamma distribution
#  It suits nicely for fits of log10(multilicity) and/or log10(chi2) distributions
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::Log10GammaDist 
#  @see Gaudi::Math::Log10GammaDist 
#  @see AnalysisModels::LogGammaDist 
#  @see Gaudi::Math::LogGammaDist 
#  @see AnalysisModels::GammaDist 
#  @see Gaudi::Math::GammaDist 
class Log10GammaDist_pdf(object) :
    """
    Gamma-distribution for log10(x), where x follows Gamma distribution
    It suits nicely for fits of log10(multilicity) and/or log10(chi2) distributions
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   k     = None     ,   ## k-parameter
                   theta = None     ) : ## theta-parameter
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'kGamma_%s'           % name ,
                               'k_{#Gamma}(%s)'      % name , None , 1 , 1.e-3 , 100 )
        self.theta = makeVar ( theta   ,
                               'tGamma_%s'           % name ,
                               '#theta_{#Gamma}(%s)' % name , None , 1 , 1.e-3 , 100 )
        
        if self.k.getMin() <= 0 :
            self.k.setMin ( 1.e-3 ) 
            logger.warning( 'Log10GammaDist_pdf: min(k)     is set %s ' % self.k.getMin() ) 
            
            if self.theta.getMin() <= 0 :
                theta.setMin ( 1.e-3 ) 
            logger.warning( 'Log10GammaDist_pdf: min(theta) is set %s ' % self.theta.getMin() ) 
            
        self.pdf  = cpp.Analysis.Models.Log10GammaDist (
            'lgd10_%s'           % name ,
            'Log10GammaDist(%s)' % name ,
            self.x                 ,
            self.k                 ,
            self.theta             )

# =============================================================================
## @class LogGamma_pdf
#  - http://arxiv.org/pdf/1005.3274
#  - Prentice, R. L. (1974). A log gamma model and its maximum likelihood
#                            estimation. Biometrika 61, 539
#  - Johnson, N. L., Kotz, S., and Balakrishnan, N. (1995). Continuous
#            univariate distributions, 2nd ed. Vol. 2. Wiley, New York.
#  - Bartlett, M. S. and G., K. M. (1946). The statistical analysis of
#                  variance-heterogeneity and the logarithmic transformation. 
#                 J. Roy. Statist. Soc. Suppl. 8, 1, 128.
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::LogGamma
#  @see Gaudi::Math::LogGamma
class LogGamma_pdf(object) :
    """
    - http://arxiv.org/pdf/1005.3274
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   nu    = None     ,   ## nu-parameter
                   lam   = None     ,   ## lambda-parameter
                   alpha = None     ) : ## nu-parameter
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.nu     = makeVar ( nu       ,
                                'nu_loggamma_%s'           % name ,
                                '#nu_{#log#Gamma}(%s)'     % name , None , 0 , -10 , 10 )
        self.lam    = makeVar ( lam      ,
                                'lambda_loggamma_%s'       % name ,
                                '#lambda_{#log#Gamma}(%s)' % name , None , 1 , -10 , 10 )
        self.alpha  = makeVar ( alpha    ,
                                'alpha_loggamma_%s'        % name ,
                                '#alpha_{#log#Gamma}(%s)'  % name , None , 1 , 1.e-3 , 100 )
        
        if self.alpha.getMin() <= 0 :
            self.alpha.setMin ( 1.e-3 ) 
            logger.warning( 'LogGamma_pdf: min(alpha) is set %s ' % self.alpha.getMin() ) 

            
        self.pdf  = cpp.Analysis.Models.LogGamma (
            'lg_%s'        % name ,
            'LogGamma(%s)' % name ,
            self.x     ,
            self.nu    ,
            self.lam   ,
            self.alpha )


# =============================================================================
## @class BetaPrime_pdf
#  http://en.wikipedia.org/wiki/Beta_prime_distribution
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Analysis::Models::BetaPrime
#  @see Gaudi::Math::BetaPrime
class BetaPrime_pdf(object) :
    """
    - http://en.wikipedia.org/wiki/Beta_prime_distribution
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   alpha = None     ,   ## alpha-parameter
                   beta  = None     ) : ## beta-parameter
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.alpha  = makeVar ( alpha    ,
                                'alpha_bp_%s'              % name ,
                                '#alpha_{#beta#prime}(%s)' % name , None , 1 , 1.e-3 , 100 )
        self.beta   = makeVar ( beta     ,
                                'beta_betaprime_%s'        % name ,
                                '#beta_{#beta#prime}(%s)'  % name , None , 1 , 1.e-3 , 100 )
        
        if self.alpha.getMin() <= 0 :
            self.alpha.setMin ( 1.e-3 ) 
            logger.warning( 'BetaPrime_pdf: min(alpha) is set %s ' % self.alpha.getMin() )
            
        if self.beta .getMin() <= 0 :
            self.beta.setMin ( 1.e-3 ) 
            logger.warning( 'BetaPrime_pdf: min(beta) is set %s ' % self.beta.getMin  () ) 

            
        self.pdf  = cpp.Analysis.Models.BetaPrime (
            'bp_%s'         % name ,
            'BetaPrime(%s)' % name ,
            self.x     ,
            self.alpha ,
            self.beta  )

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
