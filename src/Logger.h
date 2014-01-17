#pragma once

#ifdef OS_WIN
#include <sal.h>
#endif

#include "singleton.h"
#include "port.h"

namespace slib{
class LogAppender;

class AppenderItem{
public:
 LogAppender* appender;
  AppenderItem* next;
  AppenderItem(LogAppender* a) :appender(a), next(NULL){}
};

class Logger{
public:
  Logger();
#ifdef OS_WIN
  typedef _Null_terminated_ const char* SLIB_PSTR;
  void log(_In_ SLIB_PSTR file, int line, int severity, _In_ SLIB_PSTR format, ...);
#else
  // Since non-static C++ methods have an implicit this argument, the arguments of such methods should be counted from two, not one.
  void log(const char *file, int line, int severity, const char *format, ...) __attribute__((format(printf, 5, 6)));
#endif
  virtual ~Logger() throw();
private:
  AppenderItem* appenders;
};
}
typedef class slib::Singleton<slib::Logger, slib::port::Mutex> LOGGER_CLASS;
#define LOGGER (LOGGER_CLASS::instance())

/** @name Log severities
*/
#define MY_LOG_DEBUG 0
#define MY_LOG_MSG 1
#define MY_LOG_WARN 2
#define MY_LOG_ERR 3

#define MYDEBUG(...) {LOGGER->log(__FILE__, __LINE__, MY_LOG_DEBUG, __VA_ARGS__);}
#define MYERROR(...) {LOGGER->log(__FILE__, __LINE__, MY_LOG_ERR, __VA_ARGS__);}