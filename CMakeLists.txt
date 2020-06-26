cmake_minimum_required(VERSION 3.5)

project(rectangle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

add_executable(rectangle
    main.cpp
    lexer.cpp
    lexerkeyword.cpp
    lexertypestring.cpp
    driver.cpp
    parser.cpp
    parsertypestring.cpp
    ast.cpp
    option.cpp
    symbol.cpp
    symboltypestring.cpp
    util.cpp
    symbolvisitor.cpp
    object.cpp
    asmtext.cpp
    asmbin.cpp
    asmmachine.cpp
    asminstruction.cpp
    structinfo.cpp
    svgpainter.cpp
    visitor.cpp
    asmvisitor.cpp
    symboltable.cpp
    astnode.cpp
)
