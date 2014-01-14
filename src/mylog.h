#pragma once

#ifdef __GNUC__
#define SLIB_CHECK_FMT(a, b) __attribute__((format(printf, a, b)))
#define SLIB_NORETURN __attribute__((noreturn))
#else
#define SLIB_CHECK_FMT(a, b)
#define SLIB_NORETURN
#endif


/** @name Log severities
 */
#define MY_LOG_DEBUG 0
#define MY_LOG_MSG 1
#define MY_LOG_WARN 2
#define MY_LOG_ERR 3



#define MYDEBUG(...) mylog(__FILE__, __LINE__, MY_LOG_DEBUG, __VA_ARGS__)
void mylog(const char *file, int line, int severity, const char *format, ...)
    SLIB_CHECK_FMT(4, 5);

void initLogFile(const char *filename);
void closeLogFile();
