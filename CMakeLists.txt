cmake_minimum_required(VERSION 3.16)
project(cavedive)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

if(WIN32)
    add_executable(cavedive WIN32 src/main.c src/vgl_camera.c src/vgl_shaders.c src/vgl_windowing.c src/vx_meshing.c src/vx_render.c src/chunk_editor.c include/glad/src/gl.c)
elseif(APPLE)
    add_executable(cavedive MACOSX_BUNDLE src/main.c src/vgl_camera.c src/vgl_shaders.c src/vgl_windowing.c src/vx_meshing.c src/vx_render.c src/chunk_editor.c include/glad/src/gl.c)
else()
    message(FATAL_ERROR "Untested Platform")
endif()

target_include_directories(cavedive PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(cavedive PRIVATE SDL3::SDL3)
