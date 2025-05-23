cmake_minimum_required(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
project(graph3D)
if(MINGW OR LINUX)
	add_compile_options(-mavx -mtune=native -O2 -g -Wall -Wextra -Wpedantic)
endif()
if(MSVC)
	add_compile_options(-O2)
endif()
include_directories(3rdparty)
add_executable(${PROJECT_NAME} src/main.cpp
src/Entity.cpp
src/GLObjects.cpp
src/Input.cpp
src/Material.cpp
src/Mesh.cpp
src/Model.cpp
src/RenderCapabilities.cpp
src/Renderer.cpp
src/Scene.cpp
src/Shader.cpp
src/ShaderCode.cpp
src/ShaderStandard.cpp
src/stb_image_impl.cpp
src/Texture.cpp
src/Window.cpp
)
if(LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL GLEW SDL3 tbb assimp fmt sail sail-c++ sail-common)
endif()

if(MINGW)
    include(FetchContent)

    # Resolve GLEW dependency
    FetchContent_Declare(
    glew
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-glew-2.2.0-3-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(glew)
    add_library(glew::glew SHARED IMPORTED)
    set_target_properties(glew::glew PROPERTIES
    IMPORTED_IMPLIB ${glew_SOURCE_DIR}/ucrt64/lib/libglew32.dll.a
    IMPORTED_LOCATION ${glew_SOURCE_DIR}/ucrt64/bin/glew32.dll
    INTERFACE_INCLUDE_DIRECTORIES ${glew_SOURCE_DIR}/ucrt64/include
    )

    # Resolve onetbb dependency
    FetchContent_Declare(
    tbb
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-tbb-2022.0.0-1-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(tbb)
    add_library(tbb::tbb SHARED IMPORTED)
    set_target_properties(tbb::tbb PROPERTIES
    IMPORTED_IMPLIB ${tbb_SOURCE_DIR}/ucrt64/lib/libtbb12.dll.a
    IMPORTED_LOCATION ${tbb_SOURCE_DIR}/ucrt64/bin/libtbb12.dll
    INTERFACE_INCLUDE_DIRECTORIES ${tbb_SOURCE_DIR}/ucrt64/include
    )

    # Resolve SDL3 Dependency
    FetchContent_Declare(
    sdl3
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-sdl3-3.2.10-2-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(sdl3)
    add_library(sdl3::sdl3 SHARED IMPORTED)
    set_target_properties(sdl3::sdl3 PROPERTIES
    IMPORTED_IMPLIB ${sdl3_SOURCE_DIR}/ucrt64/lib/libSDL3.dll.a
    IMPORTED_LOCATION ${sdl3_SOURCE_DIR}/ucrt64/bin/SDL3.dll
    INTERFACE_INCLUDE_DIRECTORIES ${sdl3_SOURCE_DIR}/ucrt64/include
    )

    # Resolve fmt Dependency
    FetchContent_Declare(
    fmt
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-fmt-11.1.4-1-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(fmt)
    add_library(fmt::fmt SHARED IMPORTED)
    set_target_properties(fmt::fmt PROPERTIES
    IMPORTED_IMPLIB ${fmt_SOURCE_DIR}/ucrt64/lib/libfmt.dll.a
    IMPORTED_LOCATION ${fmt_SOURCE_DIR}/ucrt64/bin/libfmt-11.dll
    INTERFACE_INCLUDE_DIRECTORIES ${fmt_SOURCE_DIR}/ucrt64/include
    )

    # Resolve zlib Dependency
    FetchContent_Declare(
    zlib
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-zlib-1.3.1-1-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(zlib)
    add_library(zlib::zlib SHARED IMPORTED)
    set_target_properties(zlib::zlib PROPERTIES
    IMPORTED_IMPLIB ${zlib_SOURCE_DIR}/ucrt64/lib/libz.dll.a
    IMPORTED_LOCATION ${zlib_SOURCE_DIR}/ucrt64/bin/zlib1.dll
    INTERFACE_INCLUDE_DIRECTORIES ${zlib_SOURCE_DIR}/ucrt64/include
    )

    # Resolve minizip Dependency
    FetchContent_Declare(
    minizip
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-minizip-1.3.1-1-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(minizip)
    add_library(minizip::minizip SHARED IMPORTED)
    set_target_properties(minizip::minizip PROPERTIES
    IMPORTED_IMPLIB ${minizip_SOURCE_DIR}/ucrt64/lib/libminizip.dll.a
    IMPORTED_LOCATION ${minizip_SOURCE_DIR}/ucrt64/bin/libminizip-1.dll
    INTERFACE_INCLUDE_DIRECTORIES ${minizip_SOURCE_DIR}/ucrt64/include
    )

    # Resolve bzip Dependency
    FetchContent_Declare(
    bzip
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-bzip2-1.0.8-3-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(bzip)
    add_library(bzip::bzip SHARED IMPORTED)
    set_target_properties(bzip::bzip PROPERTIES
    IMPORTED_IMPLIB ${bzip_SOURCE_DIR}/ucrt64/lib/libbz2.dll.a
    IMPORTED_LOCATION ${bzip_SOURCE_DIR}/ucrt64/bin/libbz2-1.dll
    INTERFACE_INCLUDE_DIRECTORIES ${bzip_SOURCE_DIR}/ucrt64/include
    )

    add_library(tbb::tbbmalloc SHARED IMPORTED)
    set_target_properties(tbb::tbbmalloc PROPERTIES
    IMPORTED_IMPLIB ${tbb_SOURCE_DIR}/ucrt64/lib/libtbbmalloc.dll.a
    IMPORTED_LOCATION ${tbb_SOURCE_DIR}/ucrt64/bin/libtbbmalloc.dll
    INTERFACE_INCLUDE_DIRECTORIES ${tbb_SOURCE_DIR}/ucrt64/include
    )
    target_include_directories(${PROJECT_NAME} PRIVATE ${tbb_SOURCE_DIR}/ucrt64/include)

    # Assimp dependency
    FetchContent_Declare(
    assimp
    URL https://mirror.msys2.org/mingw/ucrt64/mingw-w64-ucrt-x86_64-assimp-5.4.3-3-any.pkg.tar.zst
    )
    FetchContent_MakeAvailable(assimp)
    add_library(assimp::assimp SHARED IMPORTED)
    set_target_properties(assimp::assimp PROPERTIES
    IMPORTED_IMPLIB ${assimp_SOURCE_DIR}/ucrt64/lib/libassimp.dll.a
    IMPORTED_LOCATION ${assimp_SOURCE_DIR}/ucrt64/bin/libassimp-5.dll
    INTERFACE_INCLUDE_DIRECTORIES ${assimp_SOURCE_DIR}/ucrt64/include
    )
    target_include_directories(${PROJECT_NAME} PRIVATE ${assimp_SOURCE_DIR}/ucrt64/include)

    find_library(STDCXX_LIB stdc++)
    get_filename_component(STDCXX_LIB_DIR ${STDCXX_LIB} PATH)
    cmake_path(GET STDCXX_LIB PARENT_PATH SYS_LIB_PATH)
    set(SYS_BIN_PATH ${SYS_LIB_PATH}/../bin)

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${SYS_BIN_PATH}/libstdc++-6.dll
    ${SYS_BIN_PATH}/libgcc_s_seh-1.dll ${SYS_BIN_PATH}/libwinpthread-1.dll
    ${SYS_BIN_PATH}/libssp-0.dll)

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${glew_SOURCE_DIR}/ucrt64/bin/glew32.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${tbb_SOURCE_DIR}/ucrt64/bin/libtbb12.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${tbb_SOURCE_DIR}/ucrt64/bin/libtbbmalloc.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${assimp_SOURCE_DIR}/ucrt64/bin/libassimp-5.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${sdl3_SOURCE_DIR}/ucrt64/bin/SDL3.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${fmt_SOURCE_DIR}/ucrt64/bin/libfmt-11.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${zlib_SOURCE_DIR}/ucrt64/bin/zlib1.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${minizip_SOURCE_DIR}/ucrt64/bin/libminizip-1.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy
            ${bzip_SOURCE_DIR}/ucrt64/bin/libbz2-1.dll
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glew::glew sdl3::sdl3 tbb::tbb assimp::assimp fmt::fmt zlib::zlib minizip::minizip bzip::bzip)

    # Install portable build
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/portable)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS glew::glew RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS sdl3::sdl3 RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS tbb::tbb RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS assimp::assimp RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS fmt::fmt RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS zlib::zlib RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS minizip::minizip RUNTIME DESTINATION bin)
    install(IMPORTED_RUNTIME_ARTIFACTS bzip::bzip RUNTIME DESTINATION bin)
    install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION resources)
    install(FILES ${glew_SOURCE_DIR}/ucrt64/share/licenses/glew/LICENSE DESTINATION licenses/glew)
    install(FILES ${tbb_SOURCE_DIR}/ucrt64/share/licenses/oneTBB/LICENSE DESTINATION licenses/tbb)
    install(FILES ${assimp_SOURCE_DIR}/ucrt64/share/licenses/assimp/LICENSE DESTINATION licenses/assimp)
    install(FILES ${sdl3_SOURCE_DIR}/ucrt64/share/licenses/SDL3/LICENSE.txt DESTINATION licenses/sdl3)
    install(FILES ${fmt_SOURCE_DIR}/ucrt64/share/licenses/fmt/LICENSE DESTINATION licenses/fmt)
    install(FILES ${zlib_SOURCE_DIR}/ucrt64/share/licenses/zlib/LICENSE DESTINATION licenses/zlib)
    install(FILES ${minizip_SOURCE_DIR}/ucrt64/share/licenses/minizip/LICENSE DESTINATION licenses/minizip)
    install(FILES ${bzip_SOURCE_DIR}/ucrt64/share/licenses/bzip2/LICENSE DESTINATION licenses/bzip2)

endif()

if(MSVC)
include(FetchContent)

    # Resolve GLFW dependency
    FetchContent_Declare(
    glfw3
    URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip
    )
    FetchContent_MakeAvailable(glfw3)
    add_library(glfw3::glfw3 SHARED IMPORTED)
    set_target_properties(glfw3::glfw3 PROPERTIES
    IMPORTED_IMPLIB ${glfw3_SOURCE_DIR}/lib-vc2022/glfw3dll.lib
    IMPORTED_LOCATION ${glfw3_SOURCE_DIR}/lib-vc2022/glfw3.dll
    INTERFACE_INCLUDE_DIRECTORIES ${glfw3_SOURCE_DIR}/include
    )

    # Resolve GLEW dependency
    FetchContent_Declare(
    glew
    URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download
    )
    FetchContent_MakeAvailable(glew)
    add_library(glew::glew SHARED IMPORTED)
    set_target_properties(glew::glew PROPERTIES
    IMPORTED_IMPLIB ${glew_SOURCE_DIR}/lib/Release/x64/glew32.lib
    IMPORTED_LOCATION ${glew_SOURCE_DIR}/bin/Release/x64/glew32.dll
    INTERFACE_INCLUDE_DIRECTORIES ${glew_SOURCE_DIR}/include
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
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION resources)
endif()
