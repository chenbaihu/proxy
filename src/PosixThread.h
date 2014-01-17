#pragma once
#include <pthread.h>
#include <memory>
class IRunnable;
class PosixThread
{
private:
  pthread_t threadID;
  pthread_attr_t attr;
  std::shared_ptr<IRunnable>  runnable_;
  enum {
    INIT,
    RUNNING,
    DEAD
  } state;
public:
  PosixThread(std::shared_ptr<IRunnable> r);
  ~PosixThread();
  virtual void run();
  virtual unsigned wait();
};
