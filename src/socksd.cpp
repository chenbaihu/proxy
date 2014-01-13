#include <iostream>
#include <memory>

#include <event2/event.h>
#include <event2/thread.h>
#include <string.h>  //memset

#include "acceptor.h"
#include "ClientHandler.h"
#include "mylog.h"

void on_newconn(struct evconnlistener *listener, evutil_socket_t sock,
                struct sockaddr *addr, int addrlen, void *ptr) {
  Acceptor *lis = (Acceptor *)ptr;
  lis->on_new(listener, sock, addr, addrlen);
}

static void my_free_event_base(event_base *ebase) {
  MYDEBUG("free event_base");
  event_base_free(ebase);
  return;
}

static void my_free_event_dns_base_imm(evdns_base *base) {
  evdns_base_free(base, 0);
}

static void my_libevent_log_cb(const char *file, int line, int severity,
                               const char *msg) {
  switch (severity) {
    case _EVENT_LOG_DEBUG:
      mylog(file, line, MY_LOG_DEBUG, "%s", msg);
      break;
    case _EVENT_LOG_MSG:
      mylog(file, line, MY_LOG_DEBUG, "%s", msg);
      break;
    case _EVENT_LOG_WARN:
      mylog(file, line, MY_LOG_DEBUG, "%s", msg);
      break;
    case _EVENT_LOG_ERR:
      mylog(file, line, MY_LOG_DEBUG, "%s", msg);
      break;
    default:
      mylog(file, line, MY_LOG_DEBUG, "%s", msg);
      break; /* never reached */
  }
}

static void logfn(const char *file, int line, int is_warn, const char *msg) {
  /* if (!is_warn && !verbose)
     return; */
  mylog(file, line, MY_LOG_DEBUG, "%s", msg);
}

int main(int argc, char *argv[]) {
  MYDEBUG("start");
  event_enable_debug_mode();
  event_set_log_callback(my_libevent_log_cb);
  // TODO:add evthread_use_windows_threads()
  evthread_use_pthreads();

  evdns_set_log_fn(logfn);
  std::shared_ptr<event_base> ebase(event_base_new(), my_free_event_base);
  std::shared_ptr<evdns_base> dns_base(evdns_base_new(ebase.get(), 1),
                                       my_free_event_dns_base_imm);
  if (!ebase) {
    MYDEBUG("create event base fail");
    return -1;
  }
  const char *port = "1080";
  evutil_addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;  // AF_UNSPEC; /* v4 or v6 is fine. */
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;     /* We want a TCP socket */
  hints.ai_flags = EVUTIL_AI_PASSIVE;  // or EVUTIL_AI_ADDRCONFIG ?
  struct addrinfo *res;
  int error;
  if ((error = evutil_getaddrinfo(NULL, port, &hints, &res)) != 0) {
    MYDEBUG("Unable to resolve port %s:%s", port, evutil_gai_strerror(error));
    return -1;
  }

  evconnlistener *listener(NULL);

  Acceptor lis(dns_base);
  for (evutil_addrinfo *ai = res; ai != NULL; ai = ai->ai_next) {
    char xhost[1024];
    char xport[64];
    if (getnameinfo(ai->ai_addr, ai->ai_addrlen, xhost, sizeof(xhost), xport,
                    sizeof(xport), NI_NUMERICSERV) == 0) {
      MYDEBUG("Listening on %s:%s", xhost, xport);
    }

    if ((listener =
             evconnlistener_new_bind(ebase.get(), on_newconn, &lis,
                                     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                     -1, ai->ai_addr, ai->ai_addrlen))) {
      MYDEBUG("create listener successfully");
      lis.evlistener.reset(listener, &evconnlistener_free);
      break;
    }
  }

  MYDEBUG("enter event loop");
  event_base_loop(ebase.get(), 0);

  // It was introduced in Libevent 2.1.1-alpha.
  // libevent_global_shutdown();
  return 0;
}
