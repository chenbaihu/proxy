#pragma once
#include <memory>

class IRunnable;

#ifdef OS_WIN
#include "Win32Thread.h"
typedef Win32Thread MyThread;
#else
#include "PosixThread.h"
typedef PosixThread MyThread;
#endif

