#include <glog/logging.h>

#include "ClientHandler.h"
#include "acceptor.h"
#include "Socks5Req.h"

void ClientHandler::close(){
    lis_->remove(id_);
  }
void ClientHandler::on_remote_read(bufferevent *bev){
  auto input=bufferevent_get_input(bev);
  size_t len=evbuffer_get_length(input);
  LOG(INFO)<<"write "<<len<<" bytes to client";
  bufferevent_write_buffer(this->bev_,input);
}
void ClientHandler::on_read(bufferevent *bev){
//TODO: state machine
    evbuffer *input = bufferevent_get_input(bev);
    event_base *base=bufferevent_get_base(bev);

    size_t n;
    bool eof=false;
    while(!eof){
      LOG(INFO)<<"state="<<state;
      switch(state){
      case VERSION:
	n=evbuffer_remove(input,&version,1);
	if(n==0) {eof=true;break;}
	else if(version==5) {
	  state=S5_NMETHODS;
	  continue;
	} else {
	  close();
	  return;
	}
	break;
      case S5_NMETHODS:
	n=evbuffer_remove(input,&nmethods,1);
	if(n==0) {eof=true;break;}
	else if(nmethods==0){
	  close();
	  return;
	} else {
	  state=S5_METHODS;
	  continue;
	}
	break;
      case S5_METHODS:
	{
	  if(evbuffer_get_length(input)<nmethods) {eof=true;break;}
	  //ignore all
	  evbuffer_drain(input,nmethods);
	  const uint8_t reply[]={0x05,0x00};
	  bufferevent_write(bev,reply,sizeof(reply));
	  state=S5_REQ;
	}
	break;
      case S5_REQ:
	{
	  if(evbuffer_get_length(input)<=6) {eof=true;break;}
	  uint8_t reqHeader[6];
	  evbuffer_copyout(input,reqHeader,sizeof(reqHeader));
	  if(reqHeader[0]!=0x5) {close(); return;}
	  uint8_t ATYP=reqHeader[3];
	  size_t REQ_len(0);
	  if(ATYP==0x01) REQ_len=6+4; 
	  else if(ATYP==0x03) REQ_len=6+reqHeader[4]+1; 
	  else if(ATYP==0x04) REQ_len=6+16; 
	  if(evbuffer_get_length(input)<REQ_len) {eof=true;break;}
	  std::unique_ptr<uint8_t[]> tmp(new uint8_t[REQ_len]);
	  evbuffer_remove(input,tmp.get(),REQ_len);
	  Socks5Req req;
	  req.parse(tmp.get(),REQ_len);
	  //TODO:bufferevent_socket_connect_hostname
	  remote_bev_=bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	  bufferevent_setcb(remote_bev_,remote_readcb,NULL,remote_eventcb,this);
	  bufferevent_socket_connect(remote_bev_,&req.addr,sizeof(sockaddr_in));
	  bufferevent_enable(remote_bev_, EV_READ); /* Start reading. */
	  eof=true;
	  state=CONNECTING;
	  break;
	}
	break;
      case CONNECTING:
	close();
	return;
      case PROXY:
	{
	  if(!remote_bev_) {close(); return;}
	  size_t len=evbuffer_get_length(input);
	  LOG(INFO)<<"write "<<len<<" bytes to remote";
	  bufferevent_write_buffer(remote_bev_,input);
	}
      default:
	eof=true;
	break;
      }
    }
  }

void ClientHandler::on_write(bufferevent *bev){
    
}
void ClientHandler::on_remote_event(bufferevent *bev,short events){
  if (events & BEV_EVENT_CONNECTED) {
    LOG(INFO)<<"remote connected";
    state=PROXY;
    uint8_t r[]={0x05,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    bufferevent_write(this->bev_,r,sizeof(r));
    return;
  } else if (events & BEV_EVENT_ERROR) {
    LOG(INFO)<<"event error";
    close();
  }
}

void ClientHandler::on_event(bufferevent *bev,short events){
  LOG(INFO)<<"event"<<events;
  if (events & BEV_EVENT_ERROR) {
    LOG(INFO)<<"event error";
  }
  close();
}

ClientHandler::~ClientHandler() throw(){
  LOG(INFO)<<"close connection "<<id_;
  if(bev_) bufferevent_free(bev_);
  if(remote_bev_) bufferevent_free(remote_bev_);
  if(readbuf) evbuffer_free(readbuf);
}
