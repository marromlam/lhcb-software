'''
Helpers for construction of Stripping Selections
'''
__author__ = "Juan Palacios juan.palacios@cern.ch"
__date__ = "30/06/2010"

__all__ = ('checkConfig',
           'LineBuilder',
           'lineNames',
           'outputLocations',
           'lineFromName'
           )

import inspect
from os import environ
from pprint import *
from Gaudi.Configuration import *
from StrippingConf.Configuration import StrippingLine

def checkConfig(reference_keys, configuration) :
    """
    Check that all the required configuration parameters are present
    in configuration.keys(), and that the number of keys in configuration
    are as expected.
    """
    absentKeys = []

    for key in reference_keys :
        if key not in configuration.keys():
            absentKeys.append(key)

    if len(absentKeys) != 0 :
        raise KeyError('Keys missing in configuration: '+ str(absentKeys))

    if len(configuration.keys()) != len(reference_keys) :
        raise KeyError('Configuration has unexpected number of parameters.')

def lineNames(lineBuilder) :
    """
    Return a list with all the names of the lines handled by a lineBuilder.
    """
    return tuple([line.name() for line in lineBuilder.lines()])

def outputLocations(lineBuilder) :
    """
    Return a list with all the output locations of the lines handled by
    a lineBuilder.
    """
    return tuple([line.outputLocation() for line in lineBuilder.lines()])

def lineFromName(lineBuilder, lineName) :
    """
    Get the line that has a given name. If not available, return None.
    """
    lines = filter(lambda x: x.name()==lineName , lineBuilder.lines())
    if len(lines)>1 :
        raise Exception("More than one line with name ", lineName)
    if len(lines) == 0 : return None
    return lines[0]

class LineBuilder(object) :

    """

    """

    __configuration_keys__ = ()

    lineNames = lineNames
    outputLocations = outputLocations
    lineFromName = lineFromName

    def __init__(self, 
                 name,
                 config) :
        
        checkConfig(self.configKeys(),
                    config)
        self._config = config
        self._name = name
        self._lines = []

    def configKeys(self) :
        return self.__configuration_keys__

    def name(self) :
        return self._name

    def configurationParameter(self, key) :
        return self._config[key]

    def registerLine(self, line) :
        self.checkLine(line)
        self._lines += [line]

    def checkLine(self, line) :
        assert type(line) == StrippingLine
        # add other checks here. For example, outputLocations.
        # Do all lines have one?
        
    def lines(self) :
        return tuple(self._lines)
    
    def keys         ( self       ) : return self.configKeys() 
    def has_key      ( self , key ) :
        return item in self.configKeys() and self._config.has_key ( key ) 
    def __getitem__  ( self , key ) : return self.configurationParameter ( key)
    def __contains__ ( self , key ) : return self.has_key ( item ) 
    
    
class MasterLineBuilder(LineBuilder):
    """
    Configure more than one line builder from the same class at once
    """
        
    def __init__(self,
                 name,
                 config,
                 SlaveBuilderClass):
        """
        config should be a dictionary or a map {SlaveSuffix: config_slave}
        """
        self.__slave_builders__={}
        self._config = config
        self._name = name
        
        if not issubclass(SlaveBuilderClass, LineBuilder):
            raise TypeError, "The slave class must inherit from a LineBuilder"
        
        self.__configuration_keys__=SlaveBuilderClass.__configuration_keys__
        
        #check all the configs
        for slaveLine in config.keys():
            try:
                config[slaveLine].keys()
            except AttributeError:
                raise KeyError, "config key "+slaveLine+" does not hold a correctly formed config for the slave"
            
            checkConfig(self.configKeys(),
                        config[slaveLine])
        
        #then build all slaves
        for slaveLine in config.keys():
            self.__slave_builders__[slaveLine]=SlaveBuilderClass(name+slaveLine,config[slaveLine])
        
    
    def lines(self):
        """
        Return a tuple of the lines from all slaves
        """
        _lines=[]
        for slave in self.__slave_builders__:
            _lines+=list(self.__slave_builders__[slave].lines())
        return tuple(_lines)
    
    #add slave helper functions
    def slaves(self):
        """
        Return a tuple of the LineBuilders which are slaves of this builder
        """
        _slaves=[]
        for slave in self.__slave_builders__:
            _slaves.append(self.__slave_builders__[slave])
        return tuple(_slaves)
    
    def slave(self,name):
        """
        Return a reference to the Builder which is a slave to this with Suffix=name
        """
        return self.__slave_builders__[name]
    
    #remove certain methods which should not be called
    def registerLine(self, line):
        raise Exception("Only the LineBuilders should register lines, not the masters ", self.name)
    
    def configurationParameter(self, key) :
        raise Exception("Only the LineBuilders have configuration parameters, not the masters ", self.name)




def getLineBuildersFromModule(confModule) :
    """
    Extract all the line builders from a given module.
    Return as a class name : class dictionary.
    """
    lbs = [getattr( confModule, x) for x in confModule.__dict__.keys()]
    lbs = filter(lambda lb : inspect.isclass(lb), lbs)
    lbs = filter(lambda lb : issubclass(lb, LineBuilder), lbs)
    lineBuilderDict = {}
    for lb in lbs :
        if lb.__name__ != LineBuilder.__name__ :
            lineBuilderDict[lb.__name__] = lb
    return lineBuilderDict

def getBuilderConfFromModule(confModule) :
    """
    Extract all the line builders from a given module.
    Return as a class name : class dictionary.
    """
    lbs = [getattr( confModule, x) for x in confModule.__dict__.keys()]
    lbs = filter(lambda lb : inspect.isclass(lb), lbs)
    lbs = filter(lambda lb : issubclass(lb, LineBuilder), lbs)
    builderConfDict = {}
    allowedkeys = ['BUILDERTYPE', 'CONFIG', 'NAME', 'STREAMS', 'WGs']
    allowedkeysshort = ['BUILDERTYPE', 'CONFIG', 'STREAMS', 'WGs']
    if confModule.__dict__.has_key('default_config'):
      if isinstance(confModule.__dict__['default_config'],dict):
        tmpKeys = confModule.default_config.keys()
        tmpKeys.sort(key=lambda x: x[0])
        if "NAME" in tmpKeys:
          for tmpKey in tmpKeys:
            if tmpKey not in allowedkeys:
              log.error("The key %(wk)s is not allowed in the default_config for %(mn)s"%{'wk':tmpKey, 'mn':confModule.__name__})
              log.error("Allowed keys are: "+",".join(allowedkeys))
              break
          if tmpKeys == allowedkeys:
            builderConfDict[confModule.default_config['NAME']] = confModule.default_config
        else:
          for k,d in confModule.default_config.iteritems():
            if not isinstance(d,dict):
              log.error("The default_config of %s.py is not a dictionary"%confModule.__name__)
              break
            tmpKeys = d.keys()
            tmpKeys.sort(key=lambda x: x[0])
            for tmpKey in tmpKeys:
              if tmpKey not in allowedkeysshort:
                log.error("The key %(wk)s is not allowed in the default_config for %(mn)s"%{'wk':tmpKey, 'mn':confModule.__name__})
                log.error("Allowed keys are: "+",".join(allowedkeysshort))
                break
            if not tmpKeys == allowedkeysshort:
              log.error("The default_config of %s.py has incorrect keys"%confModule.__name__)
              break
            builderConfDict[k] = d
    return builderConfDict

def getBuilderAndConfFromModule(confModule) :
    """
    Extract all the line builders and their default
    configuration from a given module.
    Return as a class name : class dictionary.
    """
    buildersConf = {}
    lineBuilders = getLineBuildersFromModule(confModule)
    _confs       = getBuilderConfFromModule(confModule)
    for name,conf in _confs.iteritems():  
      if conf['BUILDERTYPE'] in lineBuilders.keys():
        buildersConf[name] = conf
      else:
        print '[WARNING] The LinBuilder %s is not defined in the module %s' % (conf['BUILDERTYPE'], confModule.__name__ )

    return lineBuilder,buildersConf


