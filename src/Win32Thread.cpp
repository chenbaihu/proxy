#include "Win32Thread.h"
#include "Logger.h"

#include <process.h> //_beginthreadex

static unsigned APIENTRY my_thread_entry(void* param){
  if (param == NULL) return 1;
  IRunnable* r = (IRunnable*)param;
  unsigned ret = 1;
  try{
    ret = (unsigned)r->svc();
  } catch (std::exception& ex){
    MYERROR("thread exited,%s", ex.what());
  } catch (const char* msg){
    MYERROR("thread exited,%s", msg);
  } catch (...){
    MYERROR("thread exited with unknown error");
  }
  return ret;
}

Win32Thread::Win32Thread(std::shared_ptr<IRunnable> runnable) :runnable_(runnable), hThread(INVALID_HANDLE_VALUE){}


unsigned Win32Thread::wait(){
  if (hThread == INVALID_HANDLE_VALUE) return 0;
  WaitForSingleObject(hThread, INFINITE);
  DWORD ret;
  GetExitCodeThread(this->hThread, &ret);
  CloseHandle(hThread);
  return ret;
}

Win32Thread::~Win32Thread() throw ()
{
}

void Win32Thread::run(){
  unsigned threadID;
  this->hThread = (HANDLE)_beginthreadex(NULL, 0, my_thread_entry, runnable_.get(), 0, &threadID);
  if (this->hThread == 0){ //_beginthreadex returns 0 on failure, rather than -1L.
    this->hThread = INVALID_HANDLE_VALUE;
    return;
  }
}