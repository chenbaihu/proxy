#include "commontypes.h"

#include <memory>
#include "logger.h"
#ifdef OS_WIN
#include <windows.h>
#include <tchar.h>
#include <Strsafe.h>
#else
#include <sys/types.h>
#include <sys/stat.h> //fstat
#include <unistd.h> //fstat
#endif
namespace slib{

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





#if defined(OS_WIN)
  int64_t getFilesize(FileHandle h){
#if 0
    LARGE_INTEGER fileSize;
    if (GetFileSizeEx(h, &fileSize)){
      return fileSize.QuadPart;
    } else return -1;
#else
    BY_HANDLE_FILE_INFORMATION in;
    memset(&in, 0, sizeof(in));
    if (GetFileInformationByHandle(h, &in)) {
      return (((int64_t)in.nFileSizeHigh) << (int64_t)32) | in.nFileIndexLow;
    } else return -1;
#endif
  }
#elif defined(OS_LINUX)
  int64_t getFilesize(FileHandle h){
    struct stat st;
    memset(&st, 0, sizeof(st));
    if (fstat(h, &st)) return -1;
    return st.st_size;
  }
#endif

#if defined(OS_WIN)
  // Routine Description:
  // Retrieve and output the system error message for the last-error code
  std::string getErrorString(DWORD dw){
    if (!dw) return "no error";
    WCHAR* lpMsgBuf;    
    FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsgBuf,
      0,
      NULL);
    std::shared_ptr<WCHAR*> tmp(&lpMsgBuf, LocalFree);
    size_t oldlen=wcslen(lpMsgBuf) + 1;
    std::unique_ptr<char[]> s(new char[oldlen * 6]);
    WideCharToMultiByte(CP_UTF8, 0, lpMsgBuf, oldlen, s.get(), oldlen * 6, NULL, NULL);
    return s.get();
  }

  WCHAR* utf8_to_utf16(const char* input){
    if (input == NULL) 
      throw std::runtime_error("null pointer");
    int len=MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
    if (len <= 0)
      throw std::runtime_error("convert error");
    WCHAR* ret = new WCHAR[len];
    len=MultiByteToWideChar(CP_UTF8, 0, input, -1, ret, len);
    if (len <= 0)
      throw std::runtime_error("convert error");    
    return ret;
  }

#define THROW_WITH(X)  \
  char err_msg_buf[1024]; \
  std::string last_error_msg_ = getErrorString(GetLastError()); \
  my_snprintf(err_msg_buf, sizeof(err_msg_buf), "%s:%d %s:%s", __FILE__, (int)__LINE__, X, last_error_msg_.c_str()); \
  throw std::runtime_error(err_msg_buf);

  std::string readFileAsString(const std::string& filename ){
    std::unique_ptr<WCHAR[]> wfilename(utf8_to_utf16(filename.c_str()));
    FileHandle hFile = CreateFile(wfilename.get(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!isFileHandleValid(hFile)){
      THROW_WITH("open file fail");
    }    
    int64_t len = getFilesize(hFile);
    if (len < 0) {
      CloseHandle(hFile);
      THROW_WITH("read file fail");
    }
    std::unique_ptr<char[]> buffer(new char[(size_t)len]);
    DWORD  dwBytesRead = 0;
    if (FALSE == ::ReadFile(hFile, buffer.get(), (size_t)(len - 1), &dwBytesRead, NULL)){
      CloseHandle(hFile);
    }
    if (dwBytesRead > 0 && dwBytesRead <= len - 1){
      buffer.get()[dwBytesRead] = '\0';
      CloseHandle(hFile);
      return buffer.get();
    } else {
      CloseHandle(hFile);
      return "";
    }    
  }
#else
std::string readFileAsString(const std::string& filename ){
    throw "not impl";
  }
#endif

}

