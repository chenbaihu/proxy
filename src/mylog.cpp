

#ifdef OS_WIN
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
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

FILE* logfile=stdout;

/**
 * 打开日志文件
 * @filename
 */
void initLogFile(const char* filename){
  logfile=fopen(filename,"a+");        
}

/**
 * 关闭日志文件 
 */
void closeLogFile(){        
  if(logfile!=NULL)
    fclose(logfile);
}


int my_snprintf(char *buf, size_t buflen, const char *format, ...) {
  int r;
  va_list ap;
  va_start(ap, format);
  r = my_vsnprintf(buf, buflen, format, ap);
  va_end(ap);
  return r;
}

int my_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap) {
  int r;
  if (!buflen)
    return 0;
#ifdef _MSC_VER
  r = _vsnprintf(buf, buflen, format, ap);
  if (r < 0)
    r = _vscprintf(format, ap);
#else
  r = vsnprintf(buf, buflen, format, ap);
#endif
  buf[buflen-1] = '\0';
  return r;
}

static bool VADoRawLog(char** buf, int* size,const char* format, va_list ap) {
  int n = vsnprintf(*buf, *size, format, ap);
  if (n < 0 || n > *size) return false;
  *size -= n;
  *buf += n;
  return true;
}

static bool DoRawLog(char** buf, int* size, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = my_vsnprintf(*buf, *size, format, ap);
  va_end(ap);
  if (n < 0 || n > *size) return false;
  *size -= n;
  *buf += n;
  return true;
}


inline const char* const_basename(const char* filepath) {
  const char* base = strrchr(filepath, '/');
  if (!base)
    base = strrchr(filepath, '\\');
  return base ? (base+1) : filepath;
}

void mylog(const char* file, int line,int severity, const char* format,...){
#ifdef WIN32
  __time64_t long_time;
  _time64( &long_time ); 
  struct tm t;
  errno_t err = _localtime64_s( &t, &long_time );
  if (err){
    //VMPI_log("cannot get current time");
    return;
  }
#else
  timespec ts;
  if(clock_gettime(CLOCK_REALTIME,&ts)) return;
  struct tm t;
  localtime_r(&ts.tv_sec,&t);
#endif
  static const int kLogBufSize = 4096;
  char buffer[kLogBufSize];
  memset(buffer,0,sizeof(buffer));
  int size = sizeof(buffer);
  char* buf = buffer;
  /*DoRawLog(&buf, &size, "%02d%02d %02d:%02d:%02d %5u %s:%d]  ",                
                1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,                
                static_cast<unsigned int>(GetCurrentThreadId()),                
                const_basename(const_cast<char *>(file)), line);*/
#ifdef OS_WIN
  DoRawLog(&buf, &size, "%02d-%02d %02d:%02d:%02d %5u %s:%d ",                
	   1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,                
	   static_cast<unsigned int>(GetCurrentThreadId()),file,line);
#else
  DoRawLog(&buf, &size, "%02d-%02d %02d:%02d:%02d %s(%d): ",                
	   1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,const_basename(file),line);
#endif
  // Record the position and size of the buffer after the prefix
  const char* msg_start = buf;
  const int msg_size = size;
  va_list ap;
  va_start(ap, format);
  bool no_chop = VADoRawLog(&buf, &size, format, ap);
  va_end(ap);
  if (no_chop) {
    DoRawLog(&buf, &size, "\n");
  } else {
    DoRawLog(&buf, &size, "LOG ERROR: The Message was too long!\n");
  }        
  fputs(buffer,logfile);
}



static void _warn_helper(int severity, const char *errstr, const char *fmt,
    va_list ap);
static void my_log(int severity, const char *msg);
static void my_exit(int errcode) SLIB_NORETURN;

typedef void (*my_fatal_cb)(int err);

static my_fatal_cb fatal_fn = NULL;

void
my_set_fatal_callback(my_fatal_cb cb)
{
	fatal_fn = cb;
}

#define _MY_ERR_ABORT ((int)0xdeaddead)

static void
my_exit(int errcode)
{
	if (fatal_fn) {
		fatal_fn(errcode);
		exit(errcode); /* should never be reached */
	} else if (errcode == _MY_ERR_ABORT)
		abort();
	else
		exit(errcode);
}

void
my_err(int eval, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_warn_helper(MY_LOG_ERR, strerror(errno), fmt, ap);
	va_end(ap);
	my_exit(eval);
}

void
my_warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_warn_helper(MY_LOG_WARN, strerror(errno), fmt, ap);
	va_end(ap);
}

/*
void
my_sock_err(int eval, evutil_socket_t sock, const char *fmt, ...)
{
	va_list ap;
	int err = evutil_socket_geterror(sock);

	va_start(ap, fmt);
	_warn_helper(MY_LOG_ERR, evutil_socket_error_to_string(err), fmt, ap);
	va_end(ap);
	my_exit(eval);
}

void
my_sock_warn(evutil_socket_t sock, const char *fmt, ...)
{
	va_list ap;
	int err = evutil_socket_geterror(sock);

	va_start(ap, fmt);
	_warn_helper(MY_LOG_WARN, evutil_socket_error_to_string(err), fmt, ap);
	va_end(ap);
	}*/

void
my_errx(int eval, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_warn_helper(MY_LOG_ERR, NULL, fmt, ap);
	va_end(ap);
	my_exit(eval);
}

void my_warnx(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	_warn_helper(MY_LOG_WARN, NULL, fmt, ap);
	va_end(ap);
}

void
my_msgx(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_warn_helper(MY_LOG_MSG, NULL, fmt, ap);
	va_end(ap);
}

void my_debugx(const char *fmt, ...){
#ifdef NDEBUG
	va_list ap;
	va_start(ap, fmt);
	_warn_helper(MY_LOG_DEBUG, NULL, fmt, ap);
	va_end(ap);
#endif
}

static void
_warn_helper(int severity, const char *errstr, const char *fmt, va_list ap)
{
	char buf[1024];
	size_t len;

	if (fmt != NULL)
		my_vsnprintf(buf, sizeof(buf), fmt, ap);
	else
		buf[0] = '\0';

	if (errstr) {
		len = strlen(buf);
		if (len < sizeof(buf) - 3) {
			my_snprintf(buf + len, sizeof(buf) - len, ": %s", errstr);
		}
	}

	my_log(severity, buf);
}

static my_log_cb log_fn = NULL;

void
my_set_log_callback(my_log_cb cb)
{
	log_fn = cb;
}

static void
my_log(int severity, const char *msg)
{
	if (log_fn)
		log_fn(severity, msg);
	else {
		const char *severity_str;
		switch (severity) {
		case MY_LOG_DEBUG:
			severity_str = "debug";
			break;
		case MY_LOG_MSG:
			severity_str = "msg";
			break;
		case MY_LOG_WARN:
			severity_str = "warn";
			break;
		case MY_LOG_ERR:
			severity_str = "err";
			break;
		default:
			severity_str = "???";
			break;
		}
		(void)fprintf(stderr, "[%s] %s\n", severity_str, msg);
	}
}
