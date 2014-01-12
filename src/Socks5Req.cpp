
#include <string.h>
#include <glog/logging.h>
#include <arpa/inet.h>

#include "Socks5Req.h"

bool Socks5Req::parse(const uint8_t* buffer,size_t len){
  switch(buffer[1]){
  case 0x1:
    this->cmd=CONNECT;
    break;
  case 0x02:
    this->cmd=BIND;
    break;
  case 0x03:
    this->cmd=UDP;
    break;
  }
  size_t addrlen=0;
  memset(&addr,0,sizeof(addr));
  switch(buffer[3]){
  case 0x01:
    {
      this->addressType=IPV4;
      addrlen=4;
      if(len<6+addrlen) return false;
      sockaddr_in* paddr=(sockaddr_in*)&this->addr;
      paddr->sin_family=AF_INET;
      paddr->sin_port=*(short*)(buffer+4+addrlen);
      memcpy(&paddr->sin_addr,buffer+4,addrlen);
      
      char buf[1024];
      inet_ntop(AF_INET,&paddr->sin_addr,buf,sizeof(buf));
      LOG(INFO)<<"connecting to "<<buf<<":"<<ntohs(paddr->sin_port);      
    }
    break;
  case 0x03:
    this->addressType=DOMAINNAME;
    addrlen=buffer[4];
    if(len<6+addrlen+1) return false;
    this->portForDomain=*(short*)(buffer+5+addrlen);
    *(char*)(buffer+5+addrlen)='\0';
    this->domainName=std::string((const char*)buffer+5,addrlen+1);
    LOG(INFO)<<"connecting to "<<this->domainName;
    break;
  case 0x04: {
      this->addressType=IPV6;
      addrlen=16;
      if(len<6+addrlen) return false;
      sockaddr_in6* paddr=(sockaddr_in6*)&this->addr;
      paddr->sin6_family=AF_INET6;
      paddr->sin6_port=*(short*)(buffer+4+addrlen);
      memcpy(&paddr->sin6_addr,buffer+4,addrlen);
    }
    break;
  }
  return true;
}
