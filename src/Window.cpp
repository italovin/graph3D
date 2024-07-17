#include "Window.hpp"

void Window::ResizeCallback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

bool Window::Create(const std::string &title, int width, int height)
{
    this->title = title;
    this->width = width;
    this->height = height;
    handle = glfwCreateWindow(this->width, this->height, (this->title).c_str(), nullptr, nullptr);
    glfwSetFramebufferSizeCallback(handle, ResizeCallback);
    return handle != nullptr;
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

void Window::MakeContextCurrent(){
    glfwMakeContextCurrent(handle);
}

GLFWwindow* Window::GetHandle() const {
    return handle;
}

void Window::SetMouseCallback(GLFWcursorposfun callback){
    glfwSetCursorPosCallback(handle, callback);
}

void Window::SetKeyCallback(GLFWkeyfun callback){
    glfwSetKeyCallback(handle, callback);
}

void Window::SetMouseButtonCallback(GLFWmousebuttonfun callback){
    glfwSetMouseButtonCallback(handle, callback);
}

int Window::GetWidth() const{
    return this->width;
}

int Window::GetHeight() const{
    return this->height;
}