#pragma once
#include "singleton.h"
#include "port.h"

class Config
{
  friend slib::Singleton<Config, slib::port::Mutex>;
private:
  Config();
  ~Config();
public:
  int parseFromFile(const std::string& filename);
  bool enableEventDebug;
};

typedef class slib::Singleton<Config, slib::port::Mutex> CONFIG_CLASS;
#define CONFIG (CONFIG_CLASS::instance())
