#include <cerrno>
#include <cstdio>
#include <unistd.h>
extern "C" {
#include "dis.hxx"
}

static int id, run = 0;

static void fork_em() {
  ::dis_remove_service(id);
  ::dis_stop_serving();
  ::printf("Sleep a bit to see task disappear in did\n");
  ::sleep(3);
  pid_t pid = ::fork();
  if ( pid == 0 ) {
    ::dim_init();
    ::printf("PID:%d Register Child service Child/run to DNS....\n",::getpid());
    id = ::dis_add_service((char*)"Child/run",(char*)"I",&run,sizeof(run),0,0);
    ::dis_start_serving((char*)"Child");
  }
  else if ( pid > 0 ) {
    ::dim_init();
    ::sleep(3);
    ::printf("PID:%d RE-Register Parent to DNS....\n",::getpid());
    id = ::dis_add_service((char*)"Parent/run",(char*)"I",&run,sizeof(run),0,0);
    ::dis_start_serving((char*)"Parent");
  }      
  else {
    ::printf("ERROR in fork!!!!  %s\n",strerror(errno));
    ::exit(0);
  }
}

int main(int /* argc */, char** /* argv */) {
  id = ::dis_add_service((char*)"Parent/run",(char*)"I",&run,sizeof(run),0,0);
  ::dis_start_serving((char*)"Parent");
  ::sleep(5);
  fork_em();
  while(1) ::sleep(1);
  return 1;
}


/*

g++ -o xxxx -I../../DIM/dim -ldim -lrt -pthread ../test/test_dim.cpp

[frankm@plus04 cmt]$ ./xxxx
PID:20407 Register Child to DNS....
PID 20407 - Fri Dec  3 22:01:57 2010 - (FATAL) Child: Some Services already known to DNS
PID:20404 RE-Register Parent to DNS....

*/
