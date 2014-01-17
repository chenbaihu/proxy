#pragma once
#include "LogAppender.h"
#include <stdio.h>

namespace slib{
class FileBasedLogAppender :
  public LogAppender
{
public:
  FileBasedLogAppender();
  virtual ~FileBasedLogAppender() throw();
  void initLogFile(const char *filename);
  void closeLogFile();
  FILE *logfile;
  bool isScreenStream;
  virtual void append(const char *file, int line, int severity, const char *format, va_list ap) throw ();
};

}