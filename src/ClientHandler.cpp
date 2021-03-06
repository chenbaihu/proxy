#include <string.h>  //memset

#include "ClientHandler.h"
#include "acceptor.h"
#include "Socks5Req.h"
#include "logger.h"
#include "EvdnsWrapper.h"
/**
*  X'00' succeeded
*  X'01' general SOCKS server failure
*  X'02' connection not allowed by ruleset
*  X'03' Network unreachable
*  X'04' Host unreachable
*  X'05' Connection refused
*  X'06' TTL expired
*  X'07' Command not supported
*  X'08' Address type not supported
*  X'09' to X'FF' unassigned
*/
void ClientHandler::close() { lis_->remove(id_); }
void ClientHandler::on_remote_read(bufferevent *bev) {
  auto input = bufferevent_get_input(bev);
  size_t len = evbuffer_get_length(input);
  MYDEBUG("write %ul bytes to client", len);
  bufferevent_write_buffer(this->bev_, input);
}


void ClientHandler::on_remote_servername_resolved(
  int errcode, const std::vector<MyAddrInfo>& res) {
  if (errcode || res.empty()) {
    MYDEBUG("resolve fail %s", evutil_gai_strerror(errcode));
    return;
  } else
    MYDEBUG("resolve done");
  event_base *base = this->lis_->get_base();
  const MyAddrInfo& addr = res[0];
  remote_bev_ = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(remote_bev_, remote_readcb, NULL, remote_eventcb, this);
  bufferevent_socket_connect(remote_bev_, addr.ai_addr, addr.ai_addrlen);
  bufferevent_enable(remote_bev_, EV_READ); /* Start reading. */
  state = CONNECTING;  
}

class DNSCallback :public GetAddrCallBack{
public:
  ClientHandler* c_;
  DNSCallback(ClientHandler* c) :c_(c){}
  virtual void on(int result, const std::vector<MyAddrInfo>& res){
    c_->on_remote_servername_resolved(result, res);
  }
};

void ClientHandler::on_read(bufferevent *bev) {

  evbuffer *input = bufferevent_get_input(bev);
  size_t n;
  bool eof = false;
  while (!eof) {
    MYDEBUG("state=%d", state);
    switch (state) {
      case VERSION:
        //解析版本号
        n = evbuffer_remove(input, &version, 1);
        if (n == 0) {
          eof = true;
          break;
        } else if (version == 5) {
          state = S5_NMETHODS;
          continue;
        } else {
          close();
          return;
        }
        break;
      case S5_NMETHODS:
        n = evbuffer_remove(input, &nmethods, 1);
        if (n == 0) {
          eof = true;
          break;
        } else if (nmethods == 0) {
          close();
          return;
        } else {
          state = S5_METHODS;
          continue;
        }
        break;
      case S5_METHODS: {
        if (evbuffer_get_length(input) < nmethods) {
          eof = true;
          break;
        }
        // ignore all
        evbuffer_drain(input, nmethods);
        const uint8_t reply[] = {0x05, 0x00};
        bufferevent_write(bev, reply, sizeof(reply));
        state = S5_REQ;
      } break;
      case S5_REQ: {
        Socks5Req req;
        int r = req.parse(input);
        if (r < 0) {
          close();
          return;
        }
        if (r == 1) {
          eof = true;
          break;
        }
        std::string portstr = std::to_string(req.port);
        evutil_addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = req.addressType == Socks5Req::IPV6
                              ? AF_INET6
                              : AF_INET;  // only ipv4
        //servname一定是数字的
        hints.ai_flags = EVUTIL_AI_NUMERICSERV;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP; /* We want a TCP socket */
        if (req.addressType == Socks5Req::IPV6 ||
            req.addressType == Socks5Req::IPV4)
          hints.ai_flags |= EVUTIL_AI_NUMERICHOST;
        // hints.ai_flags = EVUTIL_AI_ADDRCONFIG;  // or EVUTIL_AI_ADDRCONFIG ?
        MYDEBUG("resolve %s", req.addr);
        lis_->dns_->async_getaddrinfo(req.addr, portstr.c_str(), &hints, new DNSCallback(this));
        
        state = RESOLVING;
        eof = true;
      } break;
      case CONNECTING:
        close();
        return;
      case PROXY: {
        if (!remote_bev_) {
          close();
          return;
        }
        size_t len = evbuffer_get_length(input);
        MYDEBUG("write %zd bytes to remote", len);
        bufferevent_write_buffer(remote_bev_, input);
      }
      default:
        eof = true;
        break;
    }
  }
}

void ClientHandler::on_write(bufferevent *bev) {}

void ClientHandler::on_remote_event(bufferevent *bev, short events) {
  if (events & BEV_EVENT_CONNECTED) {
    MYDEBUG("remote connected");
    state = PROXY;
    uint8_t r[] = {0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bufferevent_write(this->bev_, r, sizeof(r));
    return;
  } else if (events & BEV_EVENT_ERROR) {
    MYDEBUG("event error");
    close();
  } else {
    MYDEBUG("event error");
    close();
  }
}

void ClientHandler::on_event(bufferevent *bev, short events) {
  MYDEBUG("event %hd", events);
  if (events & BEV_EVENT_ERROR) {
    MYDEBUG("event error");
  }
  close();
}

ClientHandler::~ClientHandler() throw() {
  MYDEBUG("close connection %ld", id_);
  if (bev_) bufferevent_free(bev_);
  if (remote_bev_) bufferevent_free(remote_bev_);
  if (readbuf) evbuffer_free(readbuf);
}
