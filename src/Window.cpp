#include "Window.hpp"
#include <SDL2/SDL_opengl.h>

bool Window::Create(const std::string &title, int width, int height)
{
    handle = SDL_CreateWindow(title.c_str(),
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            width, height, // Resolução da janela
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(handle == nullptr)
        return false;

    this->title = title;
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    return true;
}

void Window::SetContextVersion(int majorVersion, int minorVersion, int patchVersion){
    contextMajorVersion = majorVersion >= 1 ? majorVersion : 1;
    contextMinorVersion = minorVersion >= 0 ? minorVersion : 0;
    contextPatchVersion = patchVersion >= 0 ? patchVersion : 0;
}

int Window::GetGLSLVersion(){
    if(contextMajorVersion == 2){
        if(contextMinorVersion == 0){
            return 110;
        } else if(contextMinorVersion == 1){
            return 120;
        }
    } else if(contextMajorVersion == 3){
        if(contextMinorVersion == 0){
            return 130;
        } else if(contextMinorVersion == 1){
            return 140;
        } else if(contextMinorVersion == 2){
            return 150;
        }
    }
    return contextMajorVersion * 100 + contextMinorVersion * 10;
}

SDL_GLContext Window::MakeContextCurrent(){
    return SDL_GL_CreateContext(this->handle);
}

SDL_Window* Window::GetHandle() const {
    return handle;
}

int Window::GetWidth() const{
    return this->width;
}

int Window::GetHeight() const{
    return this->height;
}