cmake_minimum_required(VERSION 3.28)
project(graph3D)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
include_directories(include)
add_executable(graph3D src/main.cpp src/VertexArray.cpp src/ShaderObject.cpp src/Shader.cpp
src/Material.cpp src/ShaderCode.cpp src/ShaderBuilder.cpp src/Input.cpp src/Window.cpp src/Mesh.cpp
src/Renderer.cpp src/Scene.cpp src/Entity.cpp)
target_link_libraries(graph3D GL GLEW glfw tbb)