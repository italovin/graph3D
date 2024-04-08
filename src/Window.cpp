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