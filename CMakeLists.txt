cmake_minimum_required(VERSION 3.28)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

add_executable(LearnOpenGL main.cpp
        glad.c
        App.cpp
        Renderer.cpp
        Shader.cpp
)

include_directories(
        C:/Users/Dusha/Documents/Libraries/glfw-3.4.bin.WIN64/include/
)
target_link_libraries(LearnOpenGL
        C:/Users/Dusha/Documents/Libraries/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib
)

include_directories(
        C:/Users/Dusha/Documents/Libraries/glad/include
)
