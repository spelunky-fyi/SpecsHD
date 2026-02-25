cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

project(SpecsHD)
add_subdirectory(3rdparty)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

option(DEV "Build with local development features" OFF)

if(DEV)
    add_compile_definitions(DEV)
endif()

add_library(
    specs SHARED
    dllmain.cpp
    inputs.h inputs.cpp
    config.h config.cpp
    sounds.h
    specs.h specs.cpp
    state.h state.cpp
    drawing.h drawing.cpp
    seeded_items.h seeded_items.cpp
    game_hooks.h game_hooks.cpp
    mods/dark_mode.h mods/dark_mode.cpp
    mods/full_spelunky.h mods/full_spelunky.cpp
    mods/biglunky.h mods/biglunky.cpp
    mods/uplunky.h mods/uplunky.cpp
    mods/tunnel_man.h mods/tunnel_man.cpp
    mods/seeded_mode.h mods/seeded_mode.cpp
    tabs/spawn_tab.h tabs/spawn_tab.cpp
    tabs/level_tab.h tabs/level_tab.cpp
    tabs/players_tab.h tabs/players_tab.cpp
    tabs/mods_tab.h tabs/mods_tab.cpp
    tabs/debug_tab.h tabs/debug_tab.cpp
    tabs/selected_tab.h tabs/selected_tab.cpp
    tabs/settings_tab.h tabs/settings_tab.cpp
)
target_link_libraries(specs PUBLIC hddll toml11)
