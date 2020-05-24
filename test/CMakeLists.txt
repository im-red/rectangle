cmake_minimum_required(VERSION 3.5)

project(rectangle_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-g -Wall -Wextra")

find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest_main)

include_directories(.)
include_directories(..)

set(RECT_SRCS
    ../lexer.cpp 
    ../lexerkeyword.cpp 
    ../lexertypestring.cpp
    ../driver.cpp
    ../parser.cpp
    ../parsertypestring.cpp
    ../ast.cpp
    ../option.cpp
    ../symbol.cpp
    ../symboltypestring.cpp
)

add_library(common
    ${RECT_SRCS}
)

add_executable(test_all
    test_lexer.cpp 
    test_driver.cpp
    test_parser.cpp
    test_symbol.cpp
)

add_executable(test_symbol
    test_symbol.cpp
)

target_link_libraries(test_all common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_symbol common ${GTEST_LIBRARIES} pthread)
