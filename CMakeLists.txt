cmake_minimum_required(VERSION 3.28)
project(glPathTracer)

set(CMAKE_CXX_STANDARD 17)


set(IMGUI_DIR C:/Users/Dusha/Documents/glPathTracer/libs/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)

add_executable(glPathTracer main.cpp
        glad.c
        App.cpp
        Renderer.cpp
        Shader.cpp
        ComputeShader.cpp
        Camera.cpp
        Camera.hpp
        ${IMGUI_SOURCES}
)

include_directories(
        C:/Users/Dusha/Documents/Libraries
)

include_directories(
        C:/Users/Dusha/Documents/Libraries/glfw-3.4.bin.WIN64/include/
)
target_link_libraries(glPathTracer
        C:/Users/Dusha/Documents/Libraries/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib
)

include_directories(
        C:/Users/Dusha/Documents/Libraries/glad/include
)

include_directories(
        C:/Users/Dusha/Documents/glPathTracer/libs/imgui
)



