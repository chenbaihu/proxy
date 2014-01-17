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


SET(SOCKS_SRC ClientHandler.cpp ClientHandler.h Config.cpp Config.h FileBasedLogAppender.cpp FileBasedLogAppender.h LibEventInit.cpp LibEventInit.h LogAppender.cpp LogAppender.h logger.cpp logger.h OS_Exit_Info.cpp OS_Exit_Info.h ObjectManager.cpp ObjectManager.h Socks5Req.cpp Socks5Req.h  acceptor.cpp acceptor.h base64.cpp base64.h cleanup.cpp cleanup.h common.cpp commontypes.h evbuffer_unitest.cpp googleinit.h listener.cpp listener.h md5.cpp md5.h my_getopt.cpp my_getopt.h mydata.cpp mydata.h port.h resource.h singleton.h MyThread.h MyThread.cpp)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
SET(SOCKS_SRC ${SOCKS_SRC} WsaModule.cpp WsaModule.h port_win32.cpp port_win32.h Win32Thread.cpp Win32Thread.h)
else()
SET(SOCKS_SRC ${SOCKS_SRC} PosixThread.cpp PosixThread.h port_posix.h port_posix.cpp)
endif()

ADD_LIBRARY(socks STATIC ${SOCKS_SRC}) #dns.h  dns.cpp

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_executable(socksd WIN32 winmain.cpp socksd.rc)
else()
add_executable(socksd socksd.cpp)
endif()
target_link_libraries(socksd socks event ${CMAKE_THREAD_LIBS_INIT} ${SOCKET_LIB})
#add_executable(curl_unitest curl_unitest.cpp)
#target_link_libraries(curl_unitest socks event gtest curl ${CMAKE_THREAD_LIBS_INIT})
#ADD_TEST(curl_unitest ${CMAKE_CURRENT_BINARY_DIR}/curl_unitest)

add_executable(Thread_unitest Thread_unitest.cpp)
target_link_libraries(Thread_unitest socks gtest  ${CMAKE_THREAD_LIBS_INIT})
ADD_TEST(Thread_unitest ${CMAKE_CURRENT_BINARY_DIR}/Thread_unitest)


#add_executable(evdns_unitest evdns_unitest.cpp)
#target_link_libraries(evdns_unitest event gtest  ${CMAKE_THREAD_LIBS_INIT})
#ADD_TEST(evdns_unitest ${CMAKE_CURRENT_BINARY_DIR}/evdns_unitest)
