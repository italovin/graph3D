#include <GLFW/glfw3.h>
#include <string>

class Window{
private:
    GLFWwindow *handle;
    std::string title;
    int width = 0;
    int height = 0;
    static void ResizeCallback(GLFWwindow *window, int width, int height);
public:
    bool Create(const std::string &title, int width, int height);
    void MakeContextCurrent();
    GLFWwindow* GetHandle() const;
    void SetMouseCallback(GLFWcursorposfun callback);
    void SetKeyCallback(GLFWkeyfun callback);
};