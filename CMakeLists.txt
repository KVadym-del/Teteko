cmake_minimum_required(VERSION 3.26)
project(Teteko)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 26)

file(GLOB SRC "src/*.cpp")

add_executable(Teteko ${SRC})
