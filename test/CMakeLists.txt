cmake_minimum_required(VERSION 3.5)

project(rectangle_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-g -Wall -Wextra")

find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest_main)

include_directories(../src)

set(RECT_SRCS
    ../src/lexer.cpp 
    ../src/lexerkeyword.cpp 
    ../src/lexertypestring.cpp
    ../src/driver.cpp
    ../src/parser.cpp
    ../src/parsertypestring.cpp
    ../src/ast.cpp
    ../src/option.cpp
    ../src/symbol.cpp
    ../src/symbolvisitor.cpp
    ../src/symboltypestring.cpp
    ../src/util.cpp
    ../src/object.cpp
    ../src/asmtext.cpp
    ../src/asmbin.cpp
    ../src/asminstruction.cpp
    ../src/asmmachine.cpp
    ../src/builtinstruct.cpp
    ../src/svgpainter.cpp
    ../src/visitor.cpp
    ../src/symboltable.cpp
    ../src/astnode.cpp
    ../src/asmvisitor.cpp
    ../src/topologicalsorter.cpp
    ../src/typeinfo.cpp
    ../src/dumpvisitor.cpp
    ../src/sourcefile.cpp
    ../src/errorprinter.cpp
    ../src/tokentypestring.cpp
)

add_library(common
    ${RECT_SRCS}
)

add_executable(test_all
    test_lexer.cpp 
    test_driver.cpp
#    test_parser.cpp
    test_symbol.cpp
    test_object.cpp
#    test_machine.cpp
    test_topologicalsorter.cpp
    test_util.cpp
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

add_executable(test_util
    test_util.cpp
)

target_link_libraries(test_all common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_symbol common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_object common ${GTEST_LIBRARIES} pthread)
#target_link_libraries(test_machine common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_topologicalsorter common ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_util common ${GTEST_LIBRARIES} pthread)
