cmake_minimum_required(VERSION 3.16)
project(SimpleIPC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
        Event.cc
        Event.h
        Main.cc
        SharedMemory.cc
        SharedMemory.h
        RingHeader.cc
        RingHeader.h
        Buffer.cc
        Buffer.h)