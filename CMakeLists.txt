cmake_minimum_required(VERSION 3.6)
project(Compiler)

set(CMAKE_C_COMPILER /usr/local/Cellar/gcc/7.2.0/bin/gcc-7)
set(CMAKE_CXX_COMPILER /usr/local/Cellar/gcc/7.2.0/bin/g++-7)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(include)

file(GLOB headers include/*.h)
file(GLOB sources source/*.cpp)

add_executable(Compiler "main.cpp" ${headers} ${sources})