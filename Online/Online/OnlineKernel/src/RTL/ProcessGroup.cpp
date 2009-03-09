
// $ID:
#include "RTL/ProcessGroup.h"

#include <cstdio>
#include "rtl_internal.h"
#ifdef USE_PTHREADS
#include <signal.h>
#include <sys/wait.h>
#endif
#include <iostream>
#include <cerrno>

using namespace std;
using namespace RTL;

/// Standard constructor
ProcessGroup::ProcessGroup() : Process(0)   {
}

/// Standard destructor
ProcessGroup::~ProcessGroup() {
}

/// Get a process by PID
Process* ProcessGroup::get(int pid) const {
  for(Processes::const_iterator i=m_procs.begin(); i!=m_procs.end();++i) {
    if ( pid == (*i)->pid() ) return *i;
  }
  return 0;
}

/// Start process
int ProcessGroup::start()    {
#ifdef __linux
  int status = 1;
  for(Processes::iterator i=m_procs.begin(); i!=m_procs.end();++i) {
    int iret = (*i)->start();
    if ( !lib_rtl_is_success(iret) ) {
      status = iret;
    }
  }
  return status;
#else
  return 0;
#endif
}

/// Wait for process group.end
int ProcessGroup::wait()    {
#ifdef __linux
  int status = 1;
  int cnt = m_procs.size();
  while(cnt>=0) {
    int status = 0;
    pid_t pid = waitpid(0,&status,0);
    if ( pid > 0 ) {
      Process* p = get(pid);
      if ( Process::debug() ) {
	cout << "Process " << p->name() << " ended.";
	if ( !p->output().empty() ) cout << " output redirected to:" << p->output();
	cout << endl;
      }
    }
    else if ( pid == -1 ) {
      return -1;
    }
  }
  return status;
#else
  return 0;
#endif
}

/// Add a new process
void ProcessGroup::add(Process* p) {
#ifdef __linux
  if ( p ) m_procs.push_back(p);
#else

#endif
}

/// Remove a process from the list
int ProcessGroup::remove(const string& name, Process** proc) {
  if ( proc ) *proc = 0;
  for(Processes::iterator i=m_procs.begin(); i!=m_procs.end();++i) {
    if ( name == (*i)->name() ) {
      if ( proc ) *proc = (*i);
      else        delete (*i);
      m_procs.erase(i);
      return 1;
    }
  }
  return 0;
}

/// Remove all processes
int ProcessGroup::removeAll() {
  for(Processes::iterator i=m_procs.begin(); i!=m_procs.end();++i)
    delete (*i);
  m_procs.clear();
  return 1;
}

#include <ctime>
#include <cmath>

extern "C" int rtl_test_process_group(int, char** ) {
  Process* p1, *p2, *p3;
  ProcessGroup pg;
  const char *a1[] = {"5",0}, *a2[]={"10",0}, *a3[]={"15",0};
  Process::setDebug(true);
  pg.add(p1=new Process("SLEEPER_1","/bin/sleep",a1));
  pg.add(p2=new Process("SLEEPER_2","/bin/sleep",a2));
  pg.add(p3=new Process("SLEEPER_3","/bin/sleep",a3));
  time_t start = ::time(0);
  pg.start();
  ::lib_rtl_sleep(200);
  cout << "Stop process " << p2->name() << endl;
  p2->stop();
  ::lib_rtl_sleep(200);
  pg.wait();
  pg.removeAll();
  time_t stop = ::time(0);
  if ( fabs(stop-start) > 14 ) {
    cout << "All processes ended" << endl;
  }
  else {
    cout << "Subprocess execution failed." << endl;
  }
  return 0;
}

// Dummy task executed by next test (rather than the stupid "sleep"
extern "C" int rtl_test_process_sleep(int argc, char** argv) {
  const char* nam = ::getenv("UTGID");
  if ( 0 == nam ) nam = "Unknown";
  if ( argc > 1 ) {
    int nsec;
    ::sscanf(argv[1],"%d",&nsec);
    ::lib_rtl_sleep(10*nsec);
    ::printf("%-12s Process starting...\n",nam);
    ::printf("%-12s arg0:%s\n",nam,argv[0]);
    ::printf("%-12s arg1:%s\n",nam,argv[1]);
    ::printf("%-12s Process sleeping for %d seconds.\n",nam,nsec);
    ::lib_rtl_sleep(1000*nsec);
    ::printf("%-12s Process exiting....\n",nam);
    return 0;
  }
  ::printf("%-12s Process exiting....  [WRONG ARGUMENTS]\n",nam);
  return 0;
}

extern "C" int rtl_test_sub_processes(int, char** ) {
  Process* p1, *p2, *p3, *p4, *p5, *p6;
  ProcessGroup pg;
  const char *a1[]={"rtl_test_process_sleep","2",0}, *a2[]={"rtl_test_process_sleep","3",0};
  const char *a3[]={"rtl_test_process_sleep","4",0}, *a4[]={"rtl_test_process_sleep","5",0};
  const char *a5[]={"rtl_test_process_sleep","6",0}, *a6[]={"rtl_test_process_sleep","7",0};
  string cmd = ::getenv("ONLINEKERNELROOT");
  cmd += "/";
  cmd += ::getenv("CMTCONFIG");
  cmd += "/test.exe";
  Process::setDebug(true);
  pg.add(p1=new Process("SLEEPER_1",cmd.c_str(),a1,"/dev/null"));
  pg.add(p2=new Process("SLEEPER_2",cmd.c_str(),a2));
  pg.add(p3=new Process("SLEEPER_3",cmd.c_str(),a3,"/dev/null"));
  pg.add(p4=new Process("SLEEPER_4",cmd.c_str(),a4));
  pg.add(p5=new Process("SLEEPER_5",cmd.c_str(),a5,"/dev/null"));
  pg.add(p6=new Process("SLEEPER_6",cmd.c_str(),a6));
  time_t start = ::time(0);
  pg.start();
  ::lib_rtl_sleep(200);
  cout << "Stop process " << p2->name() << endl;
  p2->stop();
  ::lib_rtl_sleep(100);
  cout << "Stop process " << p4->name() << endl;
  p4->stop();
  ::lib_rtl_sleep(100);
  cout << "Stop process " << p6->name() << endl;
  p6->stop();
  ::lib_rtl_sleep(100);
  pg.wait();
  pg.removeAll();
  time_t stop = ::time(0);
  if ( fabs(stop-start) >= 6 ) {
    cout << "All processes ended" << endl;
  }
  else {
    cout << "Subprocess execution failed." << endl;
  }
  return 0;
}
