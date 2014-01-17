#pragma once
#include "MyThread.h"
#include "IRunnable.h"
#include <Windows.h>

class Win32Thread :public MyThread{
private:
  IRunnable* runnable_;
  HANDLE hThread;
public:
  Win32Thread(IRunnable* runnable);
  virtual ~Win32Thread() throw ();
  virtual void run() ;
  virtual unsigned wait() ;
};


