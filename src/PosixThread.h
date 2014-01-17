#pragma once
#include "MyThread.h"
class PosixThread :
  public MyThread
{
private:
  pthread_t threadID;
public:
  PosixThread();
  ~PosixThread();
  virtual void run();
  virtual unsigned wait();
};
