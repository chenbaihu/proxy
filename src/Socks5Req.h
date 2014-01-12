#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

class Socks5Req{
public:
  enum {
    CONNECT=1,
    BIND=2,
    UDP=3
  } cmd;
  enum{
    IPV4=0x01,
    DOMAINNAME=0x03,
    IPV6=0x04
  } addressType;
  sockaddr addr;
  std::string domainName;
  short portForDomain;
  Socks5Req(){};
  bool parse(const uint8_t* buffer,size_t len);

};
