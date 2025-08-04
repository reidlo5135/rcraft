cmake_minimum_required(VERSION 3.31)
project(rcraft_map_server)

set(CMAKE_CXX_STANDARD 17)

add_library(rcraft_map_server SHARED library.cpp)
