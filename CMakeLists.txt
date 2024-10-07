cmake_minimum_required(VERSION 3.28)
project(graph3D)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
include_directories(3rdparty)
add_executable(graph3D src/main.cpp
src/VertexArray.cpp
src/ShaderObject.cpp
src/Shader.cpp
src/Material.cpp
src/ShaderCode.cpp
src/Input.cpp
src/Window.cpp
src/Mesh.cpp
src/Renderer.cpp
src/Scene.cpp
src/Entity.cpp)
if(LINUX)
    target_link_libraries(graph3D GLEW glfw GL)
endif()

if(WIN32)
    target_compile_definitions(graph3D PUBLIC GLEW_STATIC)
    target_link_libraries(graph3D glew32.a glfw3 opengl32 user32 gdi32 stdc++.a winpthread.a)
endif()
