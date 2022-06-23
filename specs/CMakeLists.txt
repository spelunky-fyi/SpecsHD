cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

project(SpecsHD)
add_subdirectory(3rdparty)

add_library(specs SHARED ui.h ui.cpp hooks.h hooks.cpp hd.h hd.cpp hd_entity.h specs.cpp specs.h dllmain.cpp)
target_link_libraries(specs PUBLIC imgui minhook)