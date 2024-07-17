#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Window{
private:
    GLFWwindow *handle;
    std::string title;
    int width = 0;
    int height = 0;
    int contextMajorVersion = 1;
    int contextMinorVersion = 0;
    int contextPatchVersion = 0;
    static void ResizeCallback(GLFWwindow *window, int width, int height);
public:
    bool Create(const std::string &title, int width, int height);
    void SetContextVersion(int majorVersion, int minorVersion, int patchVersion);
    int GetGLSLVersion();
    void MakeContextCurrent();
    GLFWwindow* GetHandle() const;
    void SetMouseCallback(GLFWcursorposfun callback);
    void SetKeyCallback(GLFWkeyfun callback);
    void SetMouseButtonCallback(GLFWmousebuttonfun callback);
    int GetWidth() const;
    int GetHeight() const;
};
#endif