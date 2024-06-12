cmake_minimum_required(VERSION 3.28)
project(glPathTracer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBS_DIR "${PROJECT_ROOT_DIR}/libs")
set(SOURCE_DIR "${PROJECT_ROOT_DIR}/src")

file(GLOB_RECURSE SOURCES
        "${SOURCE_DIR}/*.cpp"
        "${SOURCE_DIR}/*.c"
)


add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${LIBS_DIR}
        ${LIBS_DIR}/glfw-3.4.bin.WIN64/include
        ${LIBS_DIR}/glad/include
        ${LIBS_DIR}/imgui
        ${LIBS_DIR}/tinyobjloader
        ${LIBS_DIR}/stb
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${LIBS_DIR}/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib
)

message(STATUS "Project root: ${PROJECT_ROOT_DIR}")
message(STATUS "Source files: ${SOURCES}")
