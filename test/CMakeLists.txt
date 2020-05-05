cmake_minimum_required(VERSION 3.5)

project(rectangle_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-g -Wall -Wextra")

find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest_main)

include_directories(.)
include_directories(..)

add_executable(test_all
    test_lexer.cpp 
    ../lexer.cpp 
    ../lexerkeyword.cpp 
    ../lexertypestring.cpp
    test_driver.cpp
    ../driver.cpp
    test_parser.cpp
    ../parser.cpp
    ../parsertypestring.cpp
)
target_link_libraries(test_all ${GTEST_LIBRARIES} pthread)

