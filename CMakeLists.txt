cmake_minimum_required(VERSION 3.6)
project(Compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#include_directories(tests)

set(SOURCE_FILES main.cpp Scanner.cpp Scanner.h Token.cpp Token.h Parser.cpp Parser.h Error.cpp Error.h)

add_executable(Compiler ${SOURCE_FILES})