cmake_minimum_required(VERSION 3.16)
project(glPathTracer VERSION 1.0)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIBS_DIR "${PROJECT_ROOT_DIR}/third_party")
set(SOURCE_DIR "${PROJECT_ROOT_DIR}/src")

file(GLOB_RECURSE SOURCES
        "${SOURCE_DIR}/*.cpp"
        "${SOURCE_DIR}/*.c"
)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        COMPILE_WARNING_AS_ERROR ON
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${LIBS_DIR}
        ${LIBS_DIR}/glfw-3.4.bin.WIN64/include
        ${LIBS_DIR}/glad/include
        ${LIBS_DIR}/imgui
        ${LIBS_DIR}/tinyobjloader
        ${LIBS_DIR}/stb
)

add_subdirectory(${LIBS_DIR}/glfw-3.4)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

file(COPY resources shaders DESTINATION ${CMAKE_BINARY_DIR})

