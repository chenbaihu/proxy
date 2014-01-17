#pragma once
#include "MyThread.h"
#include <pthread.h>

class PosixThread :
  public MyThread
{
private:
  pthread_t threadID;
  IRunnable* runnable_;
  enum {
    INIT,
    RUNNING,
    DEAD
  } state;
public:
  PosixThread(IRunnable* r);
  ~PosixThread();
  virtual void run();
  virtual unsigned wait();
};
