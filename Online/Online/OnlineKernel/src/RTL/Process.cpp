#include <cstdio>
#include <signal.h>
#include <fcntl.h>
#ifndef _WIN32
#include <sys/wait.h>
#else
#define SIGKILL SIGTERM
#endif
#include <iostream>
#include <sstream>
#include <cerrno>
#include "RTL/rtl.h"
#include "RTL/Process.h"

using namespace std;
using namespace RTL;

namespace {
  bool s_debug = false;
}

/// Standard constructor
Process::Process() : m_pid(-1), m_status(0), m_state(INVALID)  {
}

/// Standard constructor
Process::Process(int id) : m_pid(id), m_status(0), m_state(id<=0 ? INVALID : RUNNING)  {
}

/// Constructor to start new process
Process::Process(const string& name, const string& exe, const char* a[])
  : m_pid(-1), m_status(0), m_state(INVALID), m_name(name), m_exec(exe)
{
  for(size_t i=0; a[i] != 0; ++i)
    m_args.push_back(a[i]);
}

/// Constructor to start new process
Process::Process(const string& name, const string& exe, const char* a[], const std::string& o)
  : m_pid(-1), m_status(0), m_state(INVALID), m_name(name), m_exec(exe), m_output(o)
{
  for(size_t i=0; a[i] != 0; ++i)
    m_args.push_back(a[i]);
}

/// Constructor to start new process
Process::Process(const string& name, const string& exe, const Arguments& a) 
  : m_pid(-1), m_status(0), m_state(INVALID), m_name(name), m_exec(exe), m_args(a)
{
}

/// Constructor to start new process
Process::Process(const string& name, const string& exe, const Arguments& a, const std::string& o) 
  : m_pid(-1), m_status(0), m_state(INVALID), m_name(name), m_exec(exe), m_output(o), m_args(a)
{
}

/// Standard destructor
Process::~Process()    {
}

/// Debugging flag
void Process::setDebug(bool val) {
  s_debug = val;
}

/// Access to debugging flag
bool Process::debug() {
  return s_debug;
}

/// Send signal to process specified by m_pid
int Process::sendSignal(int sig) {
#ifdef __linux
  if ( m_pid >= 0 ) {
    int ret = ::kill(m_pid, sig);
    if ( ret == 0 ) {
      return 1;
    }
  }
  return INVALID;
#else
  return 0;
#endif
}

/// Send signal to process specified by m_pid
int Process::sendSignalAll(int sig) {
#ifdef __linux
  if ( m_pid >= 0 ) {
    ::lib_rtl_output(LIB_RTL_DEBUG,"Send signal %d to %d\n",sig,int(-m_pid));
    int ret = ::kill(-m_pid, sig);
    if ( ret == 0 ) {
      return 1;
    }
  }
  return INVALID;
#else
  return 0;
#endif
}

/// Start process. Will NOT add new processes to new process group.
int Process::start()    {
  return start(false);
}

/// Start process
int Process::start(bool new_process_group)    {
  if ( m_pid >= 0 || m_state != INVALID ) {
    return 0;
  }
#ifdef __linux
  m_pid = ::fork();
  if (m_pid == 0)  {   // child
    int cnt = 0;
    string utgid;
    char **e, **env, **arg = new char*[m_args.size()+2];

    ::lib_rtl_install_printer(0,0);

    if ( new_process_group )   {
      ::setpgrp();
    }

    if ( !m_output.empty() ) {
      int fd = ::open(m_output.c_str(),O_WRONLY);
      if ( fd != -1 )  {
	::close(2);
	::close(1);
	::dup2(fd,2);
	::dup2(fd,1);
	::close(fd);
      }
    }

    arg[0] = (char*)m_name.c_str();
    for(size_t i=0; i<m_args.size();++i)
      arg[i+1] = (char*)m_args[i].c_str();
    arg[m_args.size()+1] = 0;
    for(cnt=0, e=environ; *e; ++e) ++cnt;
    env = new char*[cnt+2];
    utgid = "UTGID="+m_name;
    env[0] = (char*)utgid.c_str();
    for(e=environ,cnt=0; *e; ++e) env[++cnt] = *e;
    env[++cnt] = 0;
    if ( Process::debug() ) {
      std::stringstream str;
      ::lib_rtl_sleep(10);
      str << "Child: Starting process:" << m_name;
      //for(size_t i=0; i<m_args.size();++i)
      //  str << " " << arg[i+1];
      //str << " PID:" << ::lib_rtl_pid();
      if ( !m_output.empty() ) str << " output redirected to:" << m_output;
      str << endl;
      ::lib_rtl_output(LIB_RTL_INFO,str.str().c_str());
    }
    ::execve(m_exec.c_str(),arg,env);
    delete [] arg;
    delete [] env;
    return 0;
  }
  else if (m_pid > 0)   {  // parent process
    if ( Process::debug() ) {
      std::stringstream str;
      str << "Parent: Started process:" << m_name;
      if ( !m_output.empty() ) str << " output redirected to:" << m_output;
      str << endl;
      ::lib_rtl_output(LIB_RTL_INFO,str.str().c_str());
    }
    m_state = RUNNING;
    return 1;
  }
  cerr << "Failed to fork" << endl;
#endif
  return 0;
}

/// Send a signal to the process
int Process::signal(int signum)    {
  return sendSignal(signum);
}

/// Send a signal to the process and all its children (SIGTERM)
int Process::signalall(int signum)    {
  return sendSignalAll(signum);
}

 /// Terminate the process (SIGTERM)
int Process::stop()    {
  return sendSignal(SIGTERM);
}

/// Terminate the process and all its children (SIGTERM)
int Process::stopall()    {
  return sendSignalAll(SIGTERM);
}

/// Kill the process (SIGKILL)
int Process::kill()    {
  return sendSignal(SIGKILL);
}

/// Kill the process and all its children (SIGKILL)
int Process::killall()    {
  return sendSignalAll(SIGKILL);
}

/// Interrupt the process (SIGINT)
int Process::interrupt()    {
  return sendSignal(SIGINT);
}

/// Wait for process to terminate
int Process::wait(int flag)    {
#ifdef __linux
  int ret = -1;
  int opt = WNOHANG;
  switch(flag) {
  case WAIT_BLOCK:
    opt = WUNTRACED|WCONTINUED;
  case WAIT_NOBLOCK:
  default:
    break;
  }
  do {
    ret = ::waitpid(m_pid,&m_status,opt);
  } while (ret == -1 && errno == EINTR);
  std::stringstream str;
  if ( ret == -1 ) {
    return INVALID;
  }
  else if( WIFEXITED(m_status) )      {
    str << "waitpid() '" << m_name << "' exited: Status=" << WEXITSTATUS(m_status) << endl;
    return ENDED;
  }
  else if( WIFSTOPPED(m_status) )      {
    return STOPPED;
  }
  else if( WIFSIGNALED(m_status) )      {
    str << "waitpid() '" << m_name << "' exited due to a signal: " << WTERMSIG(m_status) << endl;
    return ENDED;
  }
  if ( str.str().length() > 0 )  {
    ::lib_rtl_output(LIB_RTL_INFO,str.str().c_str());
  }
  return RUNNING;
#else
  return 0;
#endif
}
