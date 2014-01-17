#include "WsaModule.h"
#include "logger.h"
#include "commontypes.h"

WsaModule::WsaModule(){
  WSADATA wsaData;
  //Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h 
  WORD wVersionRequested = MAKEWORD(2, 2);
  int err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    /* Tell the user that we could not find a usable */
    /* Winsock DLL.                                  */
    char buf[512];
    my_snprintf(buf, sizeof(buf), "WSAStartup failed with error: %d\n", err);
    throw std::runtime_error(buf);
  }
}


WsaModule::~WsaModule() throw(){
  WSACleanup();
}
