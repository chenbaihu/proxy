#include <iostream>
#include <memory>



#include <event2/event.h>
#include <event2/thread.h>
#include <glog/logging.h>

#include "acceptor.h"
#include "ClientHandler.h"

void on_newconn(struct evconnlistener *listener,
                          evutil_socket_t sock, struct sockaddr *addr, int addrlen, void *ptr){
  Acceptor* lis=(Acceptor*)ptr;
  lis->on_new(listener,sock,addr,addrlen);
}

static void my_free_event_base(event_base* ebase){
  LOG(INFO)<<"free event_base";
  event_base_free(ebase);
  return;
}

static void my_free_event_dns_base_imm(evdns_base *base){
  evdns_base_free(base,0);
}

static void my_libevent_log_cb(int severity, const char *msg){
  switch (severity) {


    case _EVENT_LOG_DEBUG: LOG(INFO)<<msg; break;
    case _EVENT_LOG_MSG:   LOG(INFO)<<msg;  break;
    case _EVENT_LOG_WARN:  LOG(INFO)<<msg;  break;
    case _EVENT_LOG_ERR:   LOG(ERROR)<<msg; break;
    default:               LOG(INFO)<<msg;     break; /* never reached */
  }
}

int main(int argc,char* argv[]){
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  FLAGS_logtostderr = 1;
  LOG(INFO)<<"start";
  event_set_log_callback(my_libevent_log_cb);
  //TODO:add evthread_use_windows_threads()
  evthread_use_pthreads();
  
  std::shared_ptr<event_base> ebase(event_base_new(),my_free_event_base);
  std::shared_ptr<evdns_base> dns_base(evdns_base_new(ebase.get(), 1),my_free_event_dns_base_imm);
  if(!ebase){
    LOG(ERROR)<<"create event base fail";
    return -1;
  }
  const char* port="1080";
  evutil_addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; //AF_UNSPEC; /* v4 or v6 is fine. */
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP; /* We want a TCP socket */
  hints.ai_flags = EVUTIL_AI_PASSIVE;//or EVUTIL_AI_ADDRCONFIG ?
  struct addrinfo* res;
  int error;
  if ((error = getaddrinfo(NULL, port, &hints, &res)) != 0){
    LOG(ERROR)<<"Unable to resolve port: "<<port<<": "<<gai_strerror(error)<<"\n";
    return -1;
  }
  
  evconnlistener* listener(NULL);

  Acceptor lis(dns_base);
  for (evutil_addrinfo *ai = res; ai != NULL; ai = ai->ai_next) {
    char xhost[1024];
    char xport[64];
    if (getnameinfo(ai->ai_addr, ai->ai_addrlen, xhost, sizeof(xhost), xport,
                    sizeof(xport), NI_NUMERICSERV) == 0){
      LOG(INFO)<<"Listening on "<<xhost<<":"<<xport<<".";
    }

    if((listener=evconnlistener_new_bind(ebase.get(),on_newconn,&lis,LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
                                        ai->ai_addr, ai->ai_addrlen))){
      LOG(INFO)<<"create listener successfully";
      lis.evlistener.reset(listener,&evconnlistener_free);
      break;
    }
  }
  
  LOG(INFO)<<"enter event loop";
  event_base_loop(ebase.get(),0);
  

  //It was introduced in Libevent 2.1.1-alpha.
  //libevent_global_shutdown();
  return 0;
}
