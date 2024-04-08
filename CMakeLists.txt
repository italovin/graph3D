cmake_minimum_required(VERSION 3.28)
project(graph3D)
include_directories(include)
add_executable(graph3D src/main.cpp src/SceneObject.cpp src/VertexArray.cpp src/ShaderObject.cpp src/ShaderProgram.cpp
src/ShaderBuilder.cpp src/Transform.cpp src/Camera.cpp src/Input.cpp src/Window.cpp)
target_link_libraries(graph3D GL GLEW glfw)