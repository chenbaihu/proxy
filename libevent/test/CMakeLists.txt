cmake_minimum_required (VERSION 2.8)
if(WIN32)
add_definitions(-DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DTINYTEST_LOCAL)
set(SOCKET_LIB ws2_32 shell32 advapi32)
endif()

SET(EVENT_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/regress.gen.c ${CMAKE_CURRENT_SOURCE_DIR}/regress.gen.h
   COMMAND python ${EVENT_TOP_DIR}/event_rpcgen.py ${CMAKE_CURRENT_SOURCE_DIR}/regress.rpc
   DEPENDS ${EVENT_TOP_DIR}/event_rpcgen.py
   )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)   
include_directories(${PROJECT_SOURCE_DIR}/zlib)
include_directories(${OPENSSL_INCLUDE_DIR})

SET(REGRESS_SRCS regress.c regress_buffer.c regress_http.c regress_testutils.c regress_rpc.c ${CMAKE_CURRENT_SOURCE_DIR}/regress.gen.c regress_et.c regress_bufferevent.c regress_listener.c regress_util.c tinytest.c regress_main.c regress_minheap.c regress_zlib.c regress_thread.c regress_ssl.c regress_dns.c)

if(WIN32)
SET(REGRESS_SRCS ${REGRESS_SRCS} regress_iocp.c)
endif()

add_executable (regress ${REGRESS_SRCS})
target_link_libraries (regress  event event_openssl zlibstatic ${OPENSSL_LIBRARIES}  ${SOCKET_LIB})


add_executable (test-init test-init.c)
target_link_libraries (test-init event ${SOCKET_LIB})

add_executable (test-eof test-eof.c)
target_link_libraries (test-eof event ${SOCKET_LIB})

add_executable (test-changelist test-changelist.c)
target_link_libraries (test-changelist event ${SOCKET_LIB})

add_executable (test-weof test-weof.c)
target_link_libraries (test-weof event ${SOCKET_LIB})

add_executable (test-time test-time.c)
target_link_libraries (test-time event ${SOCKET_LIB})



if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else()
add_executable (bench bench.c)
target_link_libraries (bench event ${SOCKET_LIB})

add_executable (bench_cascade bench_cascade.c)
target_link_libraries (bench_cascade event ${SOCKET_LIB})
endif()

add_executable (bench_http bench_http.c)
target_link_libraries (bench_http event ${SOCKET_LIB})

add_executable (bench_httpclient bench_httpclient.c)
target_link_libraries (bench_httpclient event ${SOCKET_LIB})

ADD_TEST(alltest ${CMAKE_CURRENT_BINARY_DIR}/test.sh)