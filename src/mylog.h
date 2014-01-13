#pragma once

#ifdef __GNUC__
#define SLIB_CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
#define SLIB_NORETURN __attribute__((noreturn))
#else
#define SLIB_CHECK_FMT(a,b)
#define SLIB_NORETURN
#endif

void my_warnx(const char *fmt, ...) SLIB_CHECK_FMT(1,2);

void my_debugx(const char *fmt, ...) SLIB_CHECK_FMT(1,2);

typedef void (*my_log_cb)(int severity, const char *msg);

/** @name Log severities
 */
#define MY_LOG_DEBUG 0
#define MY_LOG_MSG   1
#define MY_LOG_WARN  2
#define MY_LOG_ERR   3

int my_snprintf(char *buf, size_t buflen, const char *format, ...) ;
int my_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap) ;

#define MYDEBUG(...) mylog(__FILE__,__LINE__,MY_LOG_DEBUG,__VA_ARGS__)
void mylog(const char* file, int line, int severity, const char* format,...) SLIB_CHECK_FMT(4,5);

void initLogFile(const char* filename);
void closeLogFile();
