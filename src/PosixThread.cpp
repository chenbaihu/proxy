#include "PosixThread.h"


static void* my_thread_entry(void * param){
  if (param == NULL) return 1;
  IRunnable* r = (IRunnable*)param; IRunnable* r = (IRunnable*)param;
  unsigned* ret = (unsigned*)malloc(sizeof(unsigned));
  *ret = 1;
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

PosixThread::PosixThread()
{
}


PosixThread::~PosixThread()
{
}

void PosixThread::run(){
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  this->threadID = pthread_create(&threadID, &attr, my_thread_entry, runnable);
}

unsigned PosixThread::wait(){
  void *res;
  int ret = pthread_join(threadID, &res);
  if (ret)
    throw std::runtime_error("pthread_join error");
  unsigned int ret = *res;
  free(res);
  return ret;
}