cmake_minimum_required (VERSION 2.6) 
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)
ADD_LIBRARY(gtest STATIC  gtest-all.cc)
