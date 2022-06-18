# IMGUI
set(imgui_sources
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/backends/imgui_impl_dx9.cpp
        imgui/backends/imgui_impl_dx9.h
        imgui/backends/imgui_impl_win32.cpp
        imgui/backends/imgui_impl_win32.h
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_demo.cpp
        imgui/misc/cpp/imgui_stdlib.cpp)
add_library(imgui STATIC ${imgui_sources})
target_include_directories(imgui PUBLIC imgui)

# MinHook
add_subdirectory(minhook)


# Kiero
add_library(kiero STATIC kiero/kiero.h kiero/kiero.cpp)
target_include_directories(kiero PUBLIC kiero)
target_link_libraries(kiero minhook)