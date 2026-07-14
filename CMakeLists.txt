cmake_minimum_required(VERSION 3.16)
project(SimpleIPC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(flatbuffers CONFIG REQUIRED)

set(FB_SCHEMA "${CMAKE_CURRENT_SOURCE_DIR}/fbs/monster.fbs")
set(FB_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")

build_flatbuffers(
        "${FB_SCHEMA}"
        ""
        generated
        ""
        "${FB_OUTPUT_DIR}"
        ""
        ""
)

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

target_include_directories(${PROJECT_NAME} PRIVATE ${FB_OUTPUT_DIR})
add_dependencies(${PROJECT_NAME} generated)

target_link_libraries(${PROJECT_NAME} PRIVATE flatbuffers::flatbuffers)
