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
    static void ResizeCallback(GLFWwindow *window, int width, int height);
public:
    bool Create(const std::string &title, int width, int height);
    void MakeContextCurrent();
    GLFWwindow* GetHandle() const;
    void SetMouseCallback(GLFWcursorposfun callback);
    void SetKeyCallback(GLFWkeyfun callback);
    void SetMouseButtonCallback(GLFWmousebuttonfun callback);
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetProjectionMatrix(float fovy, float near, float far) const;
};