#pragma once
#include "singleton.h"
#include "port.h"

class WsaModule{
  friend class slib::Singleton<WsaModule, slib::port::Mutex>;
private:
  WsaModule();
  ~WsaModule() throw();
};
typedef class slib::Singleton<WsaModule, slib::port::Mutex> WSA_MODULE;