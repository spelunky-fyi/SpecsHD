cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

project(SpecsHD)
add_subdirectory(3rdparty)

add_library(specs SHARED hd.h hd.cpp specs.cpp specs.h dllmain.cpp)
target_link_libraries(specs PUBLIC imgui kiero)