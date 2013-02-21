#!/usr/bin/env python

import unittest

import PyCintex
#from Gaudi.Configuration import *
#import GaudiKernel.Configurable
#from GaudiKernel.Configurable import purge, applyConfigurableUsers
#from Configurables import CondDB, DDDBConf
#from Configurables import (CondDBCnvSvc,
#                           CondDBAccessSvc,
#                           CondDBDispatcherSvc,
#                           CondDBLayeringSvc,
#                           CondDBTimeSwitchSvc,
#                           CondDBSQLiteCopyAccSvc)

#import logging
#from GaudiKernel.ProcessJobOptions import InstallRootLoggingHandler
#InstallRootLoggingHandler(level = logging.DEBUG)
import sys    

import string
import random

def str_generator(size=6, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.choice(chars) for x in range(size))

class DetCondCompressionTest(unittest.TestCase):

    def assertEqualsConfig(self, lhs, rhs):
        self.assertEquals(lhs.getFullName(), rhs.getFullName())
    
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.testin = 'data'
        self.testout = 'QlpoNDFBWSZTWa/mnnIAAAEBgCQABAAgADDMDHqCcXckU4UJCv5p5yA='
    
#    def tearDown(self):
#        unittest.TestCase.tearDown(self)

    def test_compression(self):
        """Check compression method"""
        ret = PyCintex.gbl.CondDBCompression.compress(self.testin)
        self.assertEquals(self.testout, ret)
        
    def test_decompression(self):
        """Check decompression method""" 
        ret = PyCintex.gbl.CondDBCompression.decompress(self.testout)
        self.assertEquals(self.testin, ret)
        
    def test_decompression_2(self):
        """Check decompression method with random string
           should do nothing here""" 
        rdnstr = str_generator(random.randint(1,10000))
        ret = PyCintex.gbl.CondDBCompression.decompress(rdnstr)
        self.assertEquals(rdnstr, ret)
                          
    def test_both(self):
        """Check both compression and decompression methods with random string in sequence
           should return exactly the same string""" 
        rdnstr = str_generator(random.randint(1,10000))
        ret = PyCintex.gbl.CondDBCompression.compress(rdnstr)
        self.assertTrue(ret)
        ret = PyCintex.gbl.CondDBCompression.decompress(ret)
        self.assertEquals(rdnstr, ret)

if __name__ == '__main__':
    unittest.main(testRunner = unittest.TextTestRunner(stream=sys.stdout,verbosity=2))
