#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <event2/buffer.h>

class Socks5Req {
 public:
  enum {
    CONNECT = 1,
    BIND = 2,
    UDP = 3
  } cmd;
  enum {
    IPV4 = 0x01,
    DOMAINNAME = 0x03,
    IPV6 = 0x04
  } addressType;
  char addr[512];
  short port;
  Socks5Req() {};

  /**
   * \return 0,ok. -1,error. 1,need more
   */
  int parse(evbuffer *input);
};
