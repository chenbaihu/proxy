#pragma once

#include "singleton.h"
#include "port.h"
class LibEventInit
{
  friend class slib::Singleton<LibEventInit, slib::port::Mutex>;
public:
  LibEventInit();
  virtual ~LibEventInit() throw();
};

typedef class slib::Singleton<LibEventInit, slib::port::Mutex> LIBEVENTINIT;