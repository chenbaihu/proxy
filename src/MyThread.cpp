#include "MyThread.h"

#ifdef OS_WIN
#include "Win32Thread.h"
#else
#include "PosixThread.h"
#endif

MyThread::MyThread()
{
}


MyThread::~MyThread()
{
}

MyThread* MyThread::create(IRunnable* r){
#ifdef OS_WIN
  return new Win32Thread(r);
#else
  return new PosixThread(r);
#endif
}
