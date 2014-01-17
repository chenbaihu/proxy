#pragma once

#include <stdint.h>
#include <cassert>
#include <string>
#ifdef OS_WIN
#include <windows.h>
#endif

typedef int64_t ConnectionIDType;

#define MY_ROUNDUP(x, y) ((((x) + (y) - 1) / (y)) * (y))

#ifdef OS_WIN
#define MYASSERT(X) { \
	assert(X); \
	__analysis_assume(X);}
#else
#define MYASSERT(X) {std::assert(x);}
#endif


#ifdef __GNUC__
#define SLIB_CHECK_FMT(a, b) __attribute__((format(printf, a, b)))
#define SLIB_NORETURN __attribute__((noreturn))
#else
#define SLIB_CHECK_FMT(a, b)
#define SLIB_NORETURN
#endif



int my_snprintf(char *buf, size_t buflen, const char *format, ...) SLIB_CHECK_FMT(3, 4);

#ifdef OS_WIN
// These call the windows _vsnprintf, but always NUL-terminate.
inline int safe_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
  if (size == 0)        // not even room for a \0?
    return -1;          // not what C99 says to do, but what windows does
  str[size - 1] = '\0';
  return _vsnprintf(str, size - 1, format, ap);
}
#else
#define safe_vsnprintf vsnprintf
#endif

#ifdef OS_WIN
typedef HANDLE  FileHandle;
#else 
typedef int FileHandle;
#endif
inline bool isFileHandleValid(FileHandle fd){
#ifdef OS_WIN
  return fd != INVALID_HANDLE_VALUE;
#else
  return fd >= 0;
#endif
}
namespace slib{
int64_t getFilesize(FileHandle h);
std::string readFileAsString(const std::string& filename);

inline const char *const_basename(const char *filepath) {
  const char *base = strrchr(filepath, '/');
  if (!base) base = strrchr(filepath, '\\');
  return base ? (base + 1) : filepath;
}


#ifdef OS_WIN
std::string getErrorString(DWORD dw);
#endif
}