
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../linux)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
SET(SOCKET_LIB Ws2_32)
endif()

add_executable (event-test event-test.c)
target_link_libraries (event-test  event ${SOCKET_LIB})

add_executable (time-test time-test.c)
target_link_libraries (time-test  event ${SOCKET_LIB})

add_executable (signal-test signal-test.c)
target_link_libraries (signal-test  event ${SOCKET_LIB})

add_executable (dns-example dns-example.c)
target_link_libraries (dns-example  event ${SOCKET_LIB})

add_executable (hello-world hello-world.c)
target_link_libraries (hello-world  event ${SOCKET_LIB})

add_executable (http-server http-server.c)
target_link_libraries (http-server  event ${SOCKET_LIB})
