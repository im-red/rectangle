cmake_minimum_required(VERSION 3.5)

project(rectangle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(rectangle ${SRCS})
