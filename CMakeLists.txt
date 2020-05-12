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
)
