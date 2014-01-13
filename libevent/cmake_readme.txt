./configure  --prefix=/usr --sysconfdir=/etc --libdir=/usr/lib64
make include/event2/event-config.h
mv config.h linux  
mv include/event2/event-config.h linux/event2

注释掉evdns的test

修改dns的日志接口

static void _evdns_log(const char* file, int line,int warn, const char *fmt, ...) EVDNS_LOG_CHECK;
static void
_evdns_log(const char* file, int line,int warn, const char *fmt, ...)

#define log(se,...) _evdns_log(__FILE__,__LINE__,se,__VA_ARGS__)
