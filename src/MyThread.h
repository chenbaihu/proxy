#pragma once

class IRunnable;

class MyThread
{
public:
  MyThread();
  virtual ~MyThread();
  virtual void run() =0;
  virtual unsigned wait() = 0;

  static MyThread* create(IRunnable* r);
};

