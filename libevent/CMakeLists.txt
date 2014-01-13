cmake_minimum_required (VERSION 2.8)

set (VERSION_MAJOR 2)
set (VERSION_MINOR 0)



add_definitions(-DHAVE_CONFIG_H)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_definitions(-DUNICODE -D_UNICODE -DHAVE_CONFIG_H -D_EVENT_HAVE_OPENSSL=1 -D_EVENT_HAVE_OPENSSL_BIO_H=1 -D_EVENT_HAVE_LIBZ=1 -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/WIN32-Code)
SET(SYS_SRC win32select.c evthread_win32.c buffer_iocp.c event_iocp.c bufferevent_async.c signal.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/linux)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compat)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#include_directories(${PROJECT_SOURCE_DIR})

include_directories(${OPENSSL_INCLUDE_DIR})
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
SET(SYS_SRC ${SYS_SRC} select.c)
SET(SYS_SRC ${SYS_SRC} poll.c)
SET(SYS_SRC ${SYS_SRC} epoll.c)
SET(SYS_SRC ${SYS_SRC} signal.c)
SET(SYS_SRC ${SYS_SRC} evthread_pthread.c)
endif()

SET(CORE_SRCS event.c evthread.c buffer.c bufferevent.c bufferevent_sock.c bufferevent_filter.c  bufferevent_pair.c listener.c bufferevent_ratelim.c evmap.c log.c evutil.c evutil_rand.c strlcpy.c ${SYS_SRC})

SET(EXTRA_SRCS event_tagging.c http.c evdns.c evrpc.c)
SET(SSL_SRCS bufferevent_openssl.c)


ADD_LIBRARY(event_core STATIC ${CORE_SRCS})

ADD_LIBRARY(event_extras STATIC ${EXTRA_SRCS})
ADD_LIBRARY(event STATIC ${CORE_SRCS} ${EXTRA_SRCS})
ADD_LIBRARY(event_openssl STATIC ${SSL_SRCS})

ADD_SUBDIRECTORY(sample)
ADD_SUBDIRECTORY(test)
