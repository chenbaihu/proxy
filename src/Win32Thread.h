#pragma once
#include "IRunnable.h"
#include <Windows.h>
#include <memory>

class Win32Thread{
private:
  IRunnable* runnable_;
  HANDLE hThread;
public:
  Win32Thread(IRunnable* runnable);
  virtual ~Win32Thread() throw ();
  virtual void run() ;
  virtual unsigned wait() ;
};


