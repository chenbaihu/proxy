
#include <string.h>
#include <glog/logging.h>
#include <arpa/inet.h>

#include "Socks5Req.h"

int  Socks5Req::parse(evbuffer *input){
  auto len=evbuffer_get_length(input) ;
  if (len <= 6) return 1;
  uint8_t buffer[512];
  evbuffer_copyout(input, buffer, 6);
  if (buffer[0] != 0x5)  return -1;
  
  switch (buffer[1]) {
    case 0x1:
      this->cmd = CONNECT;
      break;
    case 0x02:
      this->cmd = BIND;
      break;
    case 0x03:
      this->cmd = UDP;
      break;
  }
  size_t addrlen = 0;

  switch (buffer[3]) {
    case 0x01: {
      this->addressType = IPV4;
      addrlen = 4;
      if (len < 6 + addrlen) return 1;
      evbuffer_remove(input, buffer, sizeof(buffer));
      this->port=ntohs(*(uint16_t *)(buffer + 4 + addrlen));
      evutil_inet_ntop(AF_INET,buffer + 4,this->addr,sizeof(this->addr));
    } break;
    case 0x03:
      this->addressType = DOMAINNAME;
      addrlen = buffer[4];
      if (len < 6 + addrlen + 1) return 1;
      evbuffer_remove(input, buffer, sizeof(buffer));
      this->port = ntohs(*(uint16_t *)(buffer + 5 + addrlen));
      memcpy(this->addr,buffer+5,addrlen);
      this->addr[addrlen]='\0';
      break;
    case 0x04: {
      this->addressType = IPV6;
      addrlen = 16;
      if (len < 6 + addrlen) return 1;
      evbuffer_remove(input, buffer, sizeof(buffer));
      this->port=ntohs(*(uint16_t *)(buffer + 4 + addrlen));
      evutil_inet_ntop(AF_INET6,buffer + 4,this->addr,sizeof(this->addr));
    } break;
  }
  return 0;
}
