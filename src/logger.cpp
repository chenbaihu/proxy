#include "logger.h"

#include "FileBasedLogAppender.h"
namespace slib{
Logger::Logger() {
  appenders = new AppenderItem(new slib::FileBasedLogAppender());  
}

void Logger::log(const char *file, int line, int severity, const char *format, ...){
  va_list ap;
  va_start(ap, format);
  for (AppenderItem* h = appenders; h != NULL; h = h->next){
    h->appender->append(file, line, severity, format,ap);
  }
  va_end(ap);
}

Logger::~Logger() throw(){

}
}
