cmake_minimum_required(VERSION 3.16)
project(cc_executable)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}-hello
        Event.cc
        Event.h
        Main.cc
        SharedMemory.cc
        SharedMemory.h
        RingHeader.cc
        RingHeader.h)