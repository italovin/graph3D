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

glm::mat4 Window::GetProjectionMatrix() const{
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspectiveLH(glm::radians(45.0f), (float)width/height, 0.1f, 100.0f);
    return projection;
}

glm::mat4 Window::GetProjectionMatrix(float fovy, float near, float far) const{
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspectiveLH(glm::radians(45.0f), (float)width/height, near, far);
    return projection;
}