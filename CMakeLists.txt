cmake_minimum_required(VERSION 2.8.4)
project(tp1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/matthias/ISTIC/secu/tp1/")

set(SOURCE_FILES
    my_string.c
    my_string.h
    vigenere.c)

add_executable(vigenere ${SOURCE_FILES})