cmake_minimum_required(VERSION 2.8)

PROJECT(md5brute)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
set(CMAKE_CXX_FLAGS "-g -Wall -fopenmp")
set(CMAKE_C_FLAGS "-g -Wall  -fopenmp")

add_executable(md5brute md5brute.cpp)
target_link_libraries(md5brute ssl)

INSTALL(TARGETS md5brute RUNTIME DESTINATION bin)
