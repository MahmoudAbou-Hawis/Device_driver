cmake_minimum_required(VERSION 3.5)

project(parser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(PROJECT_SOURCES
        main.cpp
        ./Parser/src/parser.cpp
)


add_executable(parser_tool ${PROJECT_SOURCES})

target_include_directories(parser_tool PUBLIC ./Parser/inc/)