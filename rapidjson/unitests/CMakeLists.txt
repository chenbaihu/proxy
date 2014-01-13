cmake_minimum_required (VERSION 2.6) 

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/rapidjson/include)


add_executable(documenttest jsoncheckertest.cpp  readertest.cpp  unittest.cpp  valuetest.cpp  writertest.cpp)
target_link_libraries(documenttest gtest  ${CMAKE_THREAD_LIBS_INIT})
ADD_TEST(documenttest ${CMAKE_CURRENT_BINARY_DIR}/documenttest)


