#include "FileBasedLogAppender.h"
#include "commontypes.h"


#include <time.h>

namespace slib{
  FileBasedLogAppender::FileBasedLogAppender() :logfile(stdout), isScreenStream(true)
  {
  }

  void FileBasedLogAppender::initLogFile(const char *filename){
    isScreenStream = false;
    logfile = fopen(filename, "a+");
  }
  void FileBasedLogAppender::closeLogFile(){
    if (!isScreenStream && logfile != NULL) fclose(logfile);
  }

  static bool DoRawLog(char **buf, int *size, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int n = safe_vsnprintf(*buf, *size, format, ap);
    va_end(ap);
    if (n < 0 || n >= *size) return false;
    *size -= n;
    *buf += n;
    return true;
  }

  void FileBasedLogAppender::append(const char *file, int line, int severity, const char *format, va_list ap) throw (){
#ifdef WIN32
    __time64_t long_time;
    _time64(&long_time);
    struct tm t;
    errno_t err = _localtime64_s(&t, &long_time);
    if (err) {
      // VMPI_log("cannot get current time");
      return;
    }
#else
    timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts)) return;
    struct tm t;
    localtime_r(&ts.tv_sec, &t);
#endif
    static const int kLogBufSize = 4096;
    char buffer[kLogBufSize];
    int size = sizeof(buffer);
    char *buf = buffer;
    /*DoRawLog(&buf, &size, "%02d%02d %02d:%02d:%02d %5u %s:%d]  ",
    1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
    static_cast<unsigned int>(GetCurrentThreadId()),
    const_basename(const_cast<char *>(file)), line);*/
#ifdef OS_WIN
    DoRawLog(&buf, &size, "%02d-%02d %02d:%02d:%02d %5u %s:%d ", 1 + t.tm_mon,
      t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
      static_cast<unsigned int>(GetCurrentThreadId()), slib::const_basename(file), line);
#else
    DoRawLog(&buf, &size, "%02d-%02d %02d:%02d:%02d %s(%d): ", 1 + t.tm_mon,
      t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, const_basename(file),
      line);
#endif
    // Record the position and size of the buffer after the prefix
    // const char* msg_start = buf;
    // const int msg_size = size;


    int n = safe_vsnprintf(buf, size, format, ap);
  
    if (n < 0 || n >= size) {
      DoRawLog(&buf, &size, "LOG ERROR: The Message was too long!\n");
    }
    else {
      size -= n;
      buf += n;
      DoRawLog(&buf, &size, "\n");
    }

    fputs(buffer, logfile);
  }

  FileBasedLogAppender::~FileBasedLogAppender() throw() {
    try{
      closeLogFile();
    } catch (...) {}
  }
}