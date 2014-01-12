cmake_minimum_required (VERSION 2.6) 
project (p5)

find_package (Threads)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckIncludeFileCXX)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
include_directories(/usr/local/include)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
MESSAGE( STATUS "build for linux")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -fno-omit-frame-pointer ") #  -fsanitize=address 
add_definitions(-DPLATFORM_POSIX -DOS_LINUX)
endif()


#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
ENABLE_TESTING()
INCLUDE(CTest)

ADD_SUBDIRECTORY(libevent)
ADD_SUBDIRECTORY(gtest)
ADD_SUBDIRECTORY(src)
