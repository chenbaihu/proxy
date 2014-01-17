#pragma once

#include <stdarg.h>

namespace slib{
  class LogAppender
  {
  public:
    LogAppender();
    virtual ~LogAppender();
    virtual void append(const char *file, int line, int severity, const char *format, va_list ap) throw () = 0;
  };

}