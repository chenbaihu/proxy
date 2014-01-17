#pragma once

#if defined(OS_LINUX)
#include "port_posix.h"
#elif defined(OS_WIN)
#include "port_win32.h"
#endif