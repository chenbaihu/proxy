#include <event2/event.h>
#include <event2/thread.h>
#include <event2/dns.h>

#include "LibEventInit.h"
#include "logger.h"
#include "Config.h"

static void my_libevent_log_cb(const char *file, int line, int severity,
  const char *msg) {
  switch (severity) {
  case _EVENT_LOG_DEBUG:
    LOGGER->log(file, line, MY_LOG_DEBUG, "%s", msg);
    break;
  case _EVENT_LOG_MSG:
    LOGGER->log(file, line, MY_LOG_DEBUG, "%s", msg);
    break;
  case _EVENT_LOG_WARN:
    LOGGER->log(file, line, MY_LOG_DEBUG, "%s", msg);
    break;
  case _EVENT_LOG_ERR:
    LOGGER->log(file, line, MY_LOG_DEBUG, "%s", msg);
    break;
  default:
    LOGGER->log(file, line, MY_LOG_DEBUG, "%s", msg);
    break; /* never reached */
  }
}

static void logfn(const char *file, int line, int is_warn, const char *msg) {
  /* if (!is_warn && !verbose)
  return; */
  LOGGER->log(file, line, MY_LOG_DEBUG, "%s", msg);
}

LibEventInit::LibEventInit()
{
  event_set_log_callback(my_libevent_log_cb);
#ifdef OS_WIN
  evthread_use_windows_threads();
#else
  evthread_use_pthreads();
#endif
  evdns_set_log_fn(logfn);

  if (CONFIG->enableEventDebug){
    MYDEBUG("enable libevent debug");
    event_enable_debug_mode();
  }
}


LibEventInit::~LibEventInit() throw(){
}
