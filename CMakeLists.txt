cmake_minimum_required(VERSION 3.13)
project(P0 C)

set(CMAKE_C_STANDARD 99)

add_executable(P0 main.c)
add_executable(test1 test1.c tool.h)