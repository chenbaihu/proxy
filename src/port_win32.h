#pragma once

#if defined(OS_MACOSX) || defined(OS_FREEBSD)
#include <machine/endian.h>
#else
//#include <endian.h>
#define LITTLE_ENDIAN
#endif

#include <stdint.h>
#include <string>
#include <Windows.h>
//#include "atomic_pointer.h"

#ifdef LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN true
#else
#define IS_LITTLE_ENDIAN (__BYTE_ORDER == __LITTLE_ENDIAN)
#endif


namespace slib {
  namespace port {

    static const bool kLittleEndian = IS_LITTLE_ENDIAN;

    class CondVar;

    class Mutex {
    public:
      Mutex();
      ~Mutex();

      void Lock();
      void Unlock();
      void AssertHeld() {}

    private:
      friend class CondVar;

      CRITICAL_SECTION cs;
      // No copying
      Mutex(const Mutex&);
      void operator=(const Mutex&);
    };

    class CondVar {
    public:
      explicit CondVar(Mutex* mu);
      ~CondVar();
      void Wait();
      void Signal();
      void SignalAll();

    private:
      CONDITION_VARIABLE cond;
      Mutex* mu_;
    };

    inline bool GetHeapProfile(void(*func)(void*, const char*, int), void* arg) {
      return false;
    }

  }  // namespace port
}  // namespace slib