#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with some simple but useful utilities
#   - timing
#   - memory
#   - suppression of stdout/stderr 
#   - dumpting of stdout/stderr into file 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-02-10
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Module with some simple but useful utilities 

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-02-10"
# =============================================================================
__all__     = (
    #
    'virtualMemory'  , ## context manager to count virtual memory increase 
    'memory'         , ## ditto
    'clocks'         , ## context manager to count clocks 
    'timing'         , ## context manager to count time 
    'timer'          , ## ditto
    #
    'tee_py'         , ## tee for Python's printouts
    'tee_cpp'        , ## tee for C++'s    printouts
    'output'         , ## redirect stdout/stderr into the file 
    'mute_py'        , ## suppress stdout/strerr Python printout 
    'silence_py'     , ## ditto 
    'mute'           , ## context manager to suppress stdout/strerr printout 
    'silence'        , ## ditto 
    'rooSilent'      , ## control RooFit verbosity 
    )    
# =============================================================================
import ROOT, time, os,sys, PyCintex   
cpp = PyCintex.makeNamespace('')
# =============================================================================
## @class Memory
#  Simple context manager to measure the virtual memory increase
#
#  @see System::virtualMemory
#  @code
#
#  with Memory() :
#     <whatever action is>
#     at the exit it prints the chaneg in virtual memory 
#  @endcode
#
# Or:
#
#  @code
#
#  with Memory() as Q :
#     <whatever action is>
#     at the exit it prints the chaneg in virtual memory
#
#  print Q.delta 
# 
#  @endcode
#
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10
class Memory(object):
    """
    Simple class to evaluate the change in virtual memory
    to be used as context manager:
    
    with Memory('here...') :
    #   <whatever action is>
    #    at the exit it prints the change in virtual memory
    
    with Memory('here...') as M :
    #   <whatever action is>
    #    at the exit it prints the change in virtual memory
    
    >>> delta = M.delta
    
    """    
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
        self.memory = cpp.System.virtualMemory()
    def __exit__  ( self, *_ ) :
        if not _ : 
            self.delta = cpp.System.virtualMemory() - self.memory
            print  '%s Memory %.1fMB ' % ( self.name , 0.001*self.delta ) 
            

# ============================================================================
## create the context manager to monitor the virtual memory increase  
def virtualMemory ( name = '' ) :
    """
    Create the context manager to monitor the virtual memory increase:

    with memory('here...') :
    #     <whatever action is>
    #      at the exit it prints the change in virtual memory
          
    with memory('here...') as m :
    #     <whatever action is>
    #      at the exit it prints the change in virtual memory
          
    >>> delta = m.delta
    
    """
    return Memory( name )

## ditto 
memory = virtualMemory  ## ditto

# =============================================================================
## @class Clock
#  Smple context manager to measure the clock counts
#
#  @code
#
#  with Clock() :
#     <whatever action is>
#     at the exit it prints the clock counts 
#  @endcode
#
# Or:
#
#  @code
#
#  with Clock() as c :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print c.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
class Clock(object):
    """
    Smple context manager to measure the clock counts
    
    with Clock() :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    with Clock() as c :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>> print c.delta 
    """
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
            self.clock = time.clock() 
    def __exit__  ( self, *_ ) :
        if not _ : 
            self.delta = time.clock() - self.clock
            print  '%s Clocks %s ' % ( self.name , self.delta ) 

# =============================================================================
## @class Timer
#  Simple context manager to measure the time 
#  @code
#
#  with Timer() :
#     <whatever action is>
#     at the exit it prints the time 
#  @endcode
#
# Or:
#
#  @code
#
#  with Timer() as t :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print ct.delta 
# 
#  @endcode
#
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10
#
class Timer(object):
    """
    Simple context manager to measure the time
    
    with Timer() :
    #     <whatever action is>
    #     at the exit it prints the time 
    
    Or:
    
    with Timer() as t :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>> print ct.delta 
    """
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
        self.time = time.time() 
    def __exit__  ( self, *_ ) :
        if not _ :
            self.delta = time.time() - self.time
            print  '%s Time   %s ' % ( self.name , self.delta ) 
            

# =============================================================================
## Simple context manager to measure the clock counts
#
#  @code
#
#  with clocks () :
#     <whatever action is>
#     at the exit it prints the clock counts
#
#  @endcode
#
# Or:
#
#  @code
#
#  with clocks () as c :
#     <whatever action is>
#     at the exist it prints the clock counts 
#
#  print c.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
def clocks ( name = '' ) :
    """
    Simple context manager to measure the clock counts 
    
    with clocks () :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    with clocks () as c :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>>print c.delta
    """
    return Clock ( name )


# =============================================================================
## Simple context manager to measure the time
#
#  @code
#
#  with timer () :
#     <whatever action is>
#     at the exit it prints the time
#
#  @endcode
#
# Or: 
#
#  @code
#
#  with timer () as t :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print t.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
def timing ( name = '' ) :
    """
    Simple context manager to measure the clock counts 
    
    with timing () :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    with timing () as c :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    >>> print c.delta
    """
    return Timer ( name )

# =============================================================================
## ditto 
timer = timing   # ditto

# ============================================================================
## @class MutePy
#  Very simple context manager to suppress python printout 
class MutePy(object):
    """
    A context manager for doing a ``deep suppression'' of stdout and stderr in 
    Python, i.e. will suppress all print, even if the print originates in a 
    compiled C/Fortran sub-function.
    This will not suppress raised exceptions, since exceptions are printed
    to stderr just before a script exits, and after the context manager has
    exited (at least, I think that is why it lets exceptions through).      
    
    stallen from  
    http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
    """
    def __init__( self , out = True , err = False ):
        self._out = out
        self._err = err
        
    def __enter__(self):
        #
        ## helper class to define empty stream 
        class Silent(object):
            def write(self,*args,**kwards) : pass

        self.stdout = sys.stdout
        self.stderr = sys.stderr
        
        if self._out : sys.stdout = Silent() 
        if self._err : sys.stderr = Silent() 

    def __exit__(self, *_):
        
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        
# ============================================================================
## @class MuteC
#  context manager to suppress pythion prinout
#  the actual code is stallen from
#  http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
#  A fix is added for "IOError: [Errno 24] Too many open files" :
#  original code leaks the file descriptors
class MuteC(object):
    """
    A context manager for doing a ``deep suppression'' of stdout and stderr in 
    Python, i.e. will suppress all print, even if the print originates in a 
    compiled C/Fortran sub-function.
    This will not suppress raised exceptions, since exceptions are printed
    to stderr just before a script exits, and after the context manager has
    exited (at least, I think that is why it lets exceptions through).      
    
    stallen from  
    http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
    """
    #
    ## class variables: dev-null device & instance counter 
    _devnull = 0
    _cnt     = 0
    
    def __init__( self , out = True , err = False ):
        
        self._out = out
        self._err = err

        # increment instance counter 
        self.__class__._cnt += 1

        # create dev-null if not done yet 
        if not self.__class__._devnull :
            self.__class__._devnull = os.open ( os.devnull , os.O_WRONLY )            

    def __del__  ( self ) :

        
        # decrement instance counter 
        self.__class__._cnt -= 1
        
        # close dev-null if not done yet 
        if self.__class__._cnt <= 0 and self.__class__._devnull : 
            os.close ( self.__class__._devnull  )
            self.__class__._devnull = 0
            

    ## context-manager 
    def __enter__(self):
        
        #
        ## Save the actual stdout (1) and stderr (2) file descriptors.
        #
        self.save_fds =  os.dup(1), os.dup(2)  # leak was here !!!
        
        #
        ## mute it!
        #
        if self._out : os.dup2 ( self.__class__._devnull , 1 )  ## C/C++
        if self._err : os.dup2 ( self.__class__._devnull , 2 )  ## C/C++

    ## context-manager 
    def __exit__(self, *_):
        
        #
        # Re-assign the real stdout/stderr back to (1) and (2)  (C/C++)
        #
        
        if self._err : os.dup2 ( self.save_fds[1] , 2 )
        if self._out : os.dup2 ( self.save_fds[0] , 1 )
        
        # fix the  file descriptor leak
        # (there were no such line in example, and it causes
        #      the sad:  "IOError: [Errno 24] Too many open files"
        
        os.close ( self.save_fds[1] ) 
        os.close ( self.save_fds[0] )
                

# =============================================================================
## dump all stdout/stderr information (including C/C++) into separate file
#  @code
#  with output ('output.txt') :
#           print 'ququ!'
#  @endcode 
#  @see MuteC 
class OutputC(object) :
    """
    Dump all stdout/stderr information into separate file:
    
    >>  with output ('output.txt') :
    ...             print 'ququ!'
    
    """

    ## constructor: file name 
    def __init__ ( self , filename , out = True , err = False ) : 
        """
        Constructor
        """
        self._out  = out 
        self._err  = out
        self._file = file ( filename , 'w' ) 
            
    ## context-manager 
    def __enter__(self):
        
        self._file.__enter__ () 
        #
        ## Save the actual stdout (1) and stderr (2) file descriptors.
        #
        self.save_fds =  os.dup(1), os.dup(2)  # leak was here !!!
        
        #
        ## mute it!
        #
        if self._out : os.dup2 ( self._file.fileno() , 1 )  ## C/C++
        if self._err : os.dup2 ( self._file.fileno() , 2 )  ## C/C++
        
    ## context-manager 
    def __exit__(self, *_):
            
        #
        # Re-assign the real stdout/stderr back to (1) and (2)  (C/C++)
        #
        
        if self._err : os.dup2 ( self.save_fds[1] , 2 )
        if self._out : os.dup2 ( self.save_fds[0] , 1 )
        
        # fix the  file descriptor leak
        # (there were no such line in example, and it causes
        #      the sad:  "IOError: [Errno 24] Too many open files"
        
        os.close ( self.save_fds[1] ) 
        os.close ( self.save_fds[0] )
        
        self._file.__exit__ ( *_ )

# =============================================================================
## very simple context manager to duplicate Python-printout into file ("tee")
#  into separate file
#  @code
#  with tee('tee.txt') :
#           print 'ququ!'
#  @endcode
#  @attention: only Python printouts are grabbed 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  date 2012-07-06
class TeePy(object) :
    """
    Very simple context manager to duplicate Python-printout into file ("tee")
    #into separate file

    >>>  with tee('tee.txt') :
    ...        print 'ququ!'
    
    Unfortunately only Python printouts are grabbed 
    """
    ## constructor 
    def __init__( self , filename ):
        
        self._file = file ( filename , 'w' ) 
    ## context manager 
    def __enter__(self):
        #
        
        self._file . __enter__ ()
        
        ## helper class to define empty stream 
        class _Tee(object):
            def __init__ ( self , the_file , the_stream ) :
                
                self._stream = the_stream 
                self._log    = the_file
                
            def write(self,*args) :
                
                self._stream .write ( *args ) 
                self._log    .write ( *args )
                
        self.stdout =  sys.stdout        
        sys.stdout  = _Tee ( self._file , self.stdout ) 
                
    ## context manager 
    def __exit__(self, *_):

        self._file.flush  ()
        self.stdout.flush ()
        
        sys.stdout = self.stdout
        
        self._file.__exit__ ( *_ )

# =============================================================================
## very simple context manager to duplicate C++-printout into file ("tee")
#  into separate file
#  @code
#  >>> with tee_cpp('tee.txt') :
#  ...         some_cpp_function() 
#  @endcode
#  @see Gaudi::Utils::Tee
#  @attention: Python&C-printouts probably  are not affected 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  date 2012-07-07
class TeeCpp(cpp.Gaudi.Utils.Tee) :
    """
    very simple context manager to duplicate C++-printout into file 
    into separate file
    
    >>> with tee_cpp('tee.txt') :
    ...         some_cpp_function()
    
    """
    def __init__ ( self , fname ) :
        cpp.Gaudi.Utils.Tee.__init__ ( self , fname )

    ## context manager
    def __enter__ ( self      ) : self.enter ()
    ## context manager
    def __exit__  ( self , *_ ) : self.exit  ()

# =============================================================================
## very simple context manager to suppress RooFit printout
#
#  @code
#
#  >>> with rooSilent( 4 , False ) :
#  ...        some_RooFit_code_here()
#
#  @endcode
#  @see RooMgsService
#  @see RooMgsService::globalKillBelow
#  @see RooMgsService::silentMode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-07-09
class RooSilent(object) :
    """
    very simple context manager to suppress RooFit printout
    
    >>> with rooSilent( 4 , False ) :
    ...        some_RooFit_code_here ()
    
    """
    ## constructor
    #  @param level  (INPUT) print level 
    #  @param silent (print level 
    # 
    def __init__ ( self , level = ROOT.RooFit.ERROR , silent = True ) :
        """
        Constructor
        
        @param level  (INPUT) print level 
        @param silent (print level 
        
        >>> with rooSilent( ROOT.RooFit.ERROR , True  ) :
        ...        some_RooFit_code_here ()
        
        
        >>> with rooSilent( ROOT.RooFit.INFO , False  ) :
        ...        some_RooFit_code_here ()
        
        
        """
        #
        if level > ROOT.RooFit.FATAL : level = ROOT.RooFit.FATAL 
        if level < ROOT.RooFit.DEBUG : level = ROOT.RooFit.DEBUG 
        #
        self._level  = level 
        self._silent = True if silent else False  
        self._svc    = ROOT.RooMsgService.instance()
        
    ## context manager
    def __enter__ ( self ) :

        self._prev_level  = self._svc.globalKillBelow  () 
        self._prev_silent = self._svc.silentMode       () 
        
        self._svc.setGlobalKillBelow  ( self._level      )
        self._svc.setSilentMode       ( self._silent     )
        
        
    ## context manager 
    def __exit__ ( self , *_ ) : 
            
        self._svc.setSilentMode      ( self._prev_silent )
        self._svc.setGlobalKillBelow ( self._prev_level  )
        

# =============================================================================
## very simple context manager to duplicate Python-printout into file ("tee")
#  into separate file
#  @code
#  >>> with tee_py ('tee.txt') :
#  ...         print 'ququ!'
#  @endcode
#  @attention: only Python prinouts are grabbed 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  date 2012-07-06
def tee_py ( filename ) :
    """
    very simple context manager to duplicate Python-printout into file ("tee")
    into separate file
    
    >>> with tee('tee.txt') :
    ...        print 'ququ!'

    Unfortunately only Python printouts are grabbed 
    """
    return TeePy ( filename ) 
    
# =============================================================================
## very simple context manager to duplicate C++-printout into file ("tee")
#  into separate file
#  @code
#  >>> with tee_cpp ('tee.txt') :
#  ...         print 'ququ!'
#  @endcode
#  @attention: only Python prinouts are grabbed 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  date 2012-07-06
def tee_cpp ( filename ) :
    """
    very simple context manager to duplicate C++-printout into file ("tee")
    into separate file
    
    >>> with tee_cpp('tee.txt') :
    ...        print 'ququ!'

    Unfortunately only Python printouts are grabbed 
    """
    return TeeCpp ( filename ) 
    
# =============================================================================
## simple context manager to redirect all (C/C++/Python) printotu
#  into separate file
#  @code
#  >>> with output ('output.txt') :
#  ...         print 'ququ!'
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  date 2012-07-06
def output ( fname , cout = True , cerr = False ) :
    """
    Simple context manager to redirect all (C/C++/Python) printotu
    
    >>> with output ('output.txt') :
    ...               print 'ququ!'
    
    """
    return OutputC ( fname  , cout , cerr )

# =============================================================================
## simple context manager to suppressC/C++-printout
#
#  @code
#  >>> with mute () :
#  ...        <some code here>
#  @endcode 
def mute ( cout = True , cerr = False )   :
    """
    Simple context manager to suppress All printout
    
    >>> with mute () :
    ...     <some code here>
    """
    return MuteC ( cout , cerr )

# =============================================================================
## simple context manager to suppress Python-printout
#
#  @code
#  >>> with mute_py () :
#  ...        <some code here>
#  @endcode 
def mute_py ( cout = True , cerr = False )   :
    """
    Simple context manager to suppress All printout
    
    >>> with mute_py () :
    ...    <some code here>
    
    """
    return MutePy ( cout , cerr )

# ==============================================================================
## ditto 
silence_py  = mute_py  # ditto
silence     = mute     # ditto


# =============================================================================
## very simple context manager to suppress RooFit printout
#
#  @code
#
#  >>> with rooSilent( 4 , False ) :
#  ...        some_RooFit_code_here()
#
#  @endcode
#  @see RooMgsService
#  @see RooMgsService::globalKillBelow
#  @see RooMgsService::silentMode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-07-09
def rooSilent ( level = ROOT.RooFit.ERROR , silent = True ) :
    """
    very simple context manager to suppress RooFit printout
    
    >>> with rooSilent( 4 , False ) :
    ...        some_RooFit_code_here()
    
    """
    return RooSilent ( level , silent ) 

# =============================================================================
## get all open file descriptors
#  The actual code is copied from http://stackoverflow.com/a/13624412
def get_open_fds():
    """
    Get all open file descriptors
    
    The actual code is copied from http://stackoverflow.com/a/13624412
    """
    #
    import resource
    import fcntl
    #
    fds = []
    soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
    for fd in range(0, soft):
        try:
            flags = fcntl.fcntl(fd, fcntl.F_GETFD)
        except IOError:
            continue
        fds.append(fd)
    return fds

# =============================================================================
## get the actual file name form file descriptor 
#  The actual code is copied from http://stackoverflow.com/a/13624412
#  @warning: it is likely to be "Linux-only" function
def get_file_names_from_file_number(fds):
    """
    Get the actual file name from file descriptor 
    The actual code is copied from http://stackoverflow.com/a/13624412 
    """
    names = []
    for fd in fds:
        names.append(os.readlink('/proc/self/fd/%d' % fd))
    return names


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
