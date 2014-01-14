cmake_minimum_required (VERSION 2.6) 

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/rapidjson/include)
include_directories(${PROJECT_SOURCE_DIR}/libevent/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
include_directories(${PROJECT_SOURCE_DIR}/libevent/linux)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
include_directories(${PROJECT_SOURCE_DIR}/libevent/WIN32-Code)
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DOS_WIN -DGTEST_HAS_TR1_TUPLE=0 -D_DEBUG -DDEBUG)
SET(SOCKET_LIB Ws2_32)
#add_definitions(-DHAVE_CONFIG_H)
endif()

ADD_LIBRARY(socks STATIC my_getopt.h my_getopt.cpp commontypes.h mylog.h mylog.cpp md5.cpp base64.cpp  acceptor.h acceptor.cpp ClientHandler.h ClientHandler.cpp mydata.h mydata.cpp Socks5Req.h Socks5Req.cpp) #dns.h  dns.cpp

add_executable(socksd socksd.cpp)
target_link_libraries(socksd socks event ${CMAKE_THREAD_LIBS_INIT} ${SOCKET_LIB})

#add_executable(curl_unitest curl_unitest.cpp)
#target_link_libraries(curl_unitest socks event gtest curl ${CMAKE_THREAD_LIBS_INIT})
#ADD_TEST(curl_unitest ${CMAKE_CURRENT_BINARY_DIR}/curl_unitest)

add_executable(evbuffer_unitest evbuffer_unitest.cpp)
target_link_libraries(evbuffer_unitest event gtest  ${CMAKE_THREAD_LIBS_INIT} ${SOCKET_LIB})
ADD_TEST(evbuffer_unitest ${CMAKE_CURRENT_BINARY_DIR}/evbuffer_unitest)


#add_executable(evdns_unitest evdns_unitest.cpp)
#target_link_libraries(evdns_unitest event gtest  ${CMAKE_THREAD_LIBS_INIT})
#ADD_TEST(evdns_unitest ${CMAKE_CURRENT_BINARY_DIR}/evdns_unitest)
