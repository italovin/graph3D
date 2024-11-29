cmake_minimum_required(VERSION 3.28)
project(graph3D)
add_compile_options(-mavx -mtune=native -O2 -Wall -Wpedantic)
include_directories(3rdparty)
add_executable(${PROJECT_NAME} src/main.cpp
src/Entity.cpp
src/GLObjects.cpp
src/Input.cpp
src/Material.cpp
src/Mesh.cpp
src/RenderCapabilities.cpp
src/Renderer.cpp
src/Scene.cpp
src/Shader.cpp
src/ShaderCode.cpp
src/ShaderStandard.cpp
src/Texture.cpp
src/Window.cpp
)
if(LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLEW glfw GL)
endif()

if(MINGW)
    include(FetchContent)

    # Resolve GLFW dependency
    FetchContent_Declare(
    glfw3
    URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip
    )
    FetchContent_MakeAvailable(glfw3)
    add_library(glfw3::glfw3 SHARED IMPORTED)
    set_target_properties(glfw3::glfw3 PROPERTIES
    IMPORTED_IMPLIB ${glfw3_SOURCE_DIR}/lib-mingw-w64/libglfw3dll.a
    IMPORTED_LOCATION ${glfw3_SOURCE_DIR}/lib-mingw-w64/glfw3.dll
    INTERFACE_INCLUDE_DIRECTORIES ${glfw3_SOURCE_DIR}/include
    )

    # Resolve GLEW dependency
    FetchContent_Declare(
    glew
    URL https://mirror.msys2.org/mingw/mingw64/mingw-w64-x86_64-glew-2.2.0-3-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(glew)
    add_library(glew::glew SHARED IMPORTED)
    set_target_properties(glew::glew PROPERTIES
    IMPORTED_IMPLIB ${glew_SOURCE_DIR}/mingw64/lib/libglew32.dll.a
    IMPORTED_LOCATION ${glew_SOURCE_DIR}/mingw64/bin/glew32.dll
    INTERFACE_INCLUDE_DIRECTORIES ${glew_SOURCE_DIR}/mingw64/include
    )
    
    find_library(STDCXX_LIB stdc++)
    get_filename_component(STDCXX_LIB_DIR ${STDCXX_LIB} PATH)
    cmake_path(GET STDCXX_LIB PARENT_PATH SYS_LIB_PATH)
    set(SYS_BIN_PATH ${SYS_LIB_PATH}/../bin)

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${SYS_BIN_PATH}/libstdc++-6.dll 
    ${SYS_BIN_PATH}/libgcc_s_seh-1.dll ${SYS_BIN_PATH}/libwinpthread-1.dll)

    target_link_libraries(${PROJECT_NAME} PRIVATE glew::glew glfw3::glfw3 opengl32)
    
    # Install portable build
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/portable)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS glew::glew RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS glfw3::glfw3 RUNTIME DESTINATION bin)
    install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION resources)
endif()
