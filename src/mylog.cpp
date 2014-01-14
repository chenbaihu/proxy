#ifdef OS_WIN
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <time.h>
#endif
#ifdef OS_LINUX
#include <time.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "mylog.h"

FILE *logfile = stdout;

/**
 * 打开日志文件
 * @filename
 */
void initLogFile(const char *filename) { logfile = fopen(filename, "a+"); }

/**
 * 关闭日志文件
 */
void closeLogFile() {
  if (logfile != NULL) fclose(logfile);
}

#ifdef OS_WIN
// These call the windows _vsnprintf, but always NUL-terminate.
int safe_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	if (size == 0)        // not even room for a \0?
		return -1;          // not what C99 says to do, but what windows does
	str[size - 1] = '\0';
	return _vsnprintf(str, size - 1, format, ap);
}
#else
#define safe_vsnprintf vsnprintf
#endif

/**
 * Do not write more than size bytes (including '\0')
 * if truncated, then return value >= buflen or the return value < 0 .
 * if not truncated, return the number of characters printed (excluding '\0'), so
 * it must less than buflen.
 */
int my_snprintf(char *buf, size_t buflen, const char *format, ...) {  
  va_list ap;
  va_start(ap, format);
  const int r = safe_vsnprintf(buf, buflen, format, ap);
  va_end(ap);
  return r;
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

inline const char *const_basename(const char *filepath) {
  const char *base = strrchr(filepath, '/');
  if (!base) base = strrchr(filepath, '\\');
  return base ? (base + 1) : filepath;
}

void mylog(const char *file, int line, int severity, const char *format, ...) {
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
		   static_cast<unsigned int>(GetCurrentThreadId()), const_basename(file), line);
#else
  DoRawLog(&buf, &size, "%02d-%02d %02d:%02d:%02d %s(%d): ", 1 + t.tm_mon,
           t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, const_basename(file),
           line);
#endif
  // Record the position and size of the buffer after the prefix
  // const char* msg_start = buf;
  // const int msg_size = size;

  va_list ap;
  va_start(ap, format);
  int n = safe_vsnprintf(buf, size, format, ap);
  va_end(ap);
  if (n < 0 || n >= size) {
	  DoRawLog(&buf, &size, "LOG ERROR: The Message was too long!\n");
  } else {	  
	  size -= n;
	  buf += n;
	  DoRawLog(&buf, &size, "\n");
  }
  
  fputs(buffer, logfile);
}