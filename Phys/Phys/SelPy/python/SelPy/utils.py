"""
General helpers for maniplating selections and lists.
"""
__author__ = "Juan PALACIOS palacios@physik.uzh.ch"

__all__ = ( 'flatSelectionList',
            'treeSelectionList',
            'flattenList',
            'isIterable',
            'removeDuplicates',
            'update_dict_overlap',
            'ClonableObject',
            'NamedObject',
            'UniquelyNamedObject',
            'NameError',
            'NonEmptyInputLocations',
            'IncompatibleInputLocations',
            'IncompatibleOutputLocation')

class ClonableObject(object) :
    """
    Base class for objects that are to be cloned with a selection of their
    constructor argument values. Clonable classes must inherit from this.
    Example:

    class Dummy(ClonableObject) :
       def __init__(self, a, b, c) :
           ClonableObject.__init__(self, locals())
           self.a = a
           self.b = b
           self.c = c

    d0 = Dummy(1,2,3)
    d1 = d0.clone(b=999)
    print d0.a, d0.b, d0.c
    print d1.a, d1.b, d1.c
    """
    def __init__(self, kwargs) :
        self._ctor_dict = dict(kwargs)
        if 'self' in self._ctor_dict :
            del self._ctor_dict['self']

    def clone(self, **kwargs) :
        new_dict = update_dict_overlap(self._ctor_dict, kwargs)
        return self.__class__(**new_dict)

class NamedObject(object) :

    def __init__(self, name) :
        self._name = name
    
    def name(self) :
        return self._name

class UniquelyNamedObject(NamedObject) :
    __used_names = []
    def __init__(self, name) :
        if name in UniquelyNamedObject.__used_names :
            raise NameError('Name ' + name + ' has already been used. Pick a new one.')
        NamedObject.__init__(self, name)
        UniquelyNamedObject.__used_names.append(name)

class SelectionBase(object) :

    def __init__(self, algorithm, outputLocation, requiredSelections) :
        self._algorithm = algorithm
        self._outputLocation = outputLocation
        self._requiredSelections = list(requiredSelections)

    def algorithm(self) :
        return self._algorithm

    def outputLocation(self) :
        return self._outputLocation

    def requiredSelections(self) :
        return list(self._requiredSelections)

class CloneCallable(object) :
    """
    Simple wrapper class to make an object's call method equivalent to
    its clone method
    """
    def __init__(self, clonable) :
        self._clonable = clonable
    def __call__(self, *args, **kwargs) :
        return self._clonable.clone(*args, **kwargs)
    
def treeSelectionList(selection) :
    """
    Return a nested list with all the selections needed by a selection, including itself.
    """
    _selList = [treeSelectionList(sel) for sel in selection.requiredSelections()] + [selection]
    return _selList

def isIterable(obj) :
    '''
    Test if an object is iterable but not a string type.
    '''
    return hasattr(obj, "__iter__") and not isinstance(obj, basestring)

def flattenList(tree) :
    """flattenList(tree) -> flat list
    Return a flat list containing all the non-iterable elements retrieved tree and tree's sub-trees.
    """
    flatList = []
    for member in tree :
        if isIterable(member) :
            flatList.extend(flattenList(member))
        else :
            flatList.append(member)
    return flatList

def removeDuplicates(obj_list) :
    """
    Remove all but the first instance of an object from the a list.
    Remove all NoneType instances.
    """
    clean_list = []
    for obj in obj_list :
        if obj not in clean_list and obj != None :
            clean_list.append(obj)
    return clean_list

def flatSelectionList(selection) :
    """
    Return a flat selection list containing all the selections required
    by selection, in the correct order of execution. Duplicates are removed,
    keeping the first one.
    """
    return removeDuplicates(flattenList(treeSelectionList(selection)))

def update_dict_overlap(dict0, dict1) :
    """
    Replace entries from dict0 with those from dict1 that have
    keys present in dict0.
    """
    overlap_keys = filter(dict1.has_key, dict0.keys())
    result = dict(dict0)
    for key in overlap_keys : result[key] = dict1[key]
    return result


def compatibleSequences( seq0, seq1) :
    if len(seq0) != len(seq1) :
        return False
    for x in seq0 :
        if x not in seq1 :
            return False
    return True


def connectToRequiredSelections(selection, inputSetter) :
    """
    Make selection get input data from its requiredSelections via an inputSetter method (str).
    """
    _outputLocations = [sel.outputLocation() for sel in selection.requiredSelections()]
    _outputLocations = filter(lambda s : s != '', _outputLocations)
    configurable = selection.algorithm()

    if inputSetter and hasattr(configurable, inputSetter) :
        _inputLocations = getattr(configurable, inputSetter)
        if len(_inputLocations) != 0 :
            if not compatibleSequences(_outputLocations,
                                       _inputLocations) :
                raise IncompatibleInputLocations('InputLocations of input algorithm incompatible with RequiredSelections!'\
                                                 '\nInputLocations: '+str(_inputLocations)+\
                                                 '\nRequiredSelections: '+str(_outputLocations))

    if inputSetter :
        configurable.__setattr__(inputSetter, list(_outputLocations))


def makeOutputLocation(name, branch, leaf) :
    """
    Create an output location path in the form branch/name/leaf.
    Check against double '/' or ending with '/'.
    """
    _outputLocation = name
    if branch != '' :            
        _outputLocation = branch + '/' + _outputLocation
    if leaf != '' :
        _outputLocation = _outputLocation + '/' + leaf
        _outputLocation.replace('//','/')
    if _outputLocation.endswith('/') :
        _outputLocation = _outputLocation[:_outputLocation.rfind('/')]
    return _outputLocation

def setOutputLocation(selection, setter) :
    configurable = selection.algorithm()
    selOutput = selection.outputLocation()
    if setter and hasattr(configurable, setter) :
        confOutput=getattr(configurable, setter)
        if confOutput != selOutput :
            raise IncompatibleOutputLocation('Output of input algorithm incompatible with Selection!'\
                                             '\nOutput: '+confOutput+\
                                             '\nSelection.outputLocation(): '+selOutput)
    if setter :
        configurable.__setattr__( setter, selOutput )
                              
class NameError(Exception) :
    pass

class NonEmptyInputLocations(Exception) :
    pass
class IncompatibleInputLocations(Exception) :
    pass
class IncompatibleOutputLocation(Exception) :
    pass

                                
