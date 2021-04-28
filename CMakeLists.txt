cmake_minimum_required(VERSION 3.20.1)

project(parser-combinator VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_subdirectory(test)