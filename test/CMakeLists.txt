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
    ../symbolvisitor.cpp
    ../symboltypestring.cpp
    ../util.cpp
    ../object.cpp
    ../asmtext.cpp
    ../asmbin.cpp
    ../asminstruction.cpp
    ../asmmachine.cpp
    ../structinfo.cpp
    ../svgpainter.cpp
    ../visitor.cpp
    ../symboltable.cpp
    ../astnode.cpp
    ../asmvisitor.cpp
    ../topologicalsorter.cpp
    ../typeinfo.cpp
)

add_library(common
    ${RECT_SRCS}
)

add_executable(test_all
    test_lexer.cpp 
    test_driver.cpp
    test_parser.cpp
    test_symbol.cpp
    test_object.cpp
#    test_machine.cpp
    test_topologicalsorter.cpp
)

add_executable(test_symbol
    test_symbol.cpp
)

add_executable(test_object
    test_object.cpp
)

#add_executable(test_machine
#    test_machine.cpp
#)

add_executable(test_topologicalsorter
    test_topologicalsorter.cpp
)

target_link_libraries(test_all common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_symbol common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_object common ${GTEST_LIBRARIES} pthread)
#target_link_libraries(test_machine common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_topologicalsorter common ${GTEST_LIBRARIES} pthread)
