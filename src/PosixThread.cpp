#include <stdlib.h>
#include <stdexcept>
#include "logger.h"
#include "PosixThread.h"
#include "IRunnable.h"

static void* my_thread_entry(void * param){
  unsigned* ret = (unsigned*)malloc(sizeof(unsigned));
  *ret = 1;
  if (param == NULL) return ret;
  IRunnable* r = (IRunnable*)param; 
  try{
    *ret = (unsigned)r->svc();
  } catch (std::exception& ex){
    MYERROR("thread exited,%s", ex.what());
  } catch (const char* msg){
    MYERROR("thread exited,%s", msg);
  } catch (...){
    MYERROR("thread exited with unknown error");
  }
  return ret;
}

PosixThread::PosixThread(std::shared_ptr<IRunnable> r):runnable_(r),state(INIT){
  pthread_attr_init(&attr);
}

PosixThread::~PosixThread()
{
}

void PosixThread::run(){
  if(state!=INIT) return;


  int err=pthread_create(&threadID, &attr, my_thread_entry, runnable_.get());
  if(err)
    return;
  state=RUNNING;
}

unsigned PosixThread::wait(){
  if(state!=RUNNING) return 1;
  void *res;
  int joinret = pthread_join(threadID, &res);
  if (joinret)
    throw std::runtime_error("pthread_join error");
  unsigned int ret = *(unsigned*)res;
  free(res);
  state=DEAD;
  return ret;
}
