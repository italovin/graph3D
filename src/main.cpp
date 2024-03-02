#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "SceneObject.hpp"
#include <string>
#include <set>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::cout << glGetString(GL_VERSION) << std::endl;

    GLint no_of_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);

    GLenum err=glewInit();
    if(err != GLEW_OK){
        std::cout << "Failed to start GLEW" << std::endl;
        return -1;
    }
       
    if(GLEW_ARB_direct_state_access)
        std::cout << "Direct access extension suported" << std::endl;

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0, 1.0f, 0.0f
    };
    unsigned int indices[] = { 0, 1, 2};
    
    SceneObject triangle = SceneObject(1);
    triangle.StartImmutableBufferStorage(0, vertices, sizeof(vertices));
    triangle.StartElementBufferStorage(indices, sizeof(indices));
    triangle.AttachVertexBuffer(0, 0, 0, 3*sizeof(float));
    triangle.AttachElementBuffer();
    triangle.SetAttribute(0, 0, 3, GL_FLOAT, GL_FALSE, 0);

    std::string triangleVertexShaderPath = "../resources/basic.vert";
    std::string triangleFragmentShaderPath = "../resources/basic.frag";
    ShaderObject triangleVertexShader = ShaderObject(GL_VERTEX_SHADER, triangleVertexShaderPath);
    ShaderObject triangleFragmentShader = ShaderObject(GL_FRAGMENT_SHADER, triangleFragmentShaderPath);
    ShaderProgram shader = ShaderProgram();
    shader.AttachShaderObject(triangleVertexShader);
    shader.AttachShaderObject(triangleFragmentShader);
    shader.Link();
    shader.RemoveShaderObject(triangleVertexShader);
    shader.RemoveShaderObject(triangleFragmentShader);

    triangle.SetShader(shader);
    

    glClearColor(0, 0, 0, 1);
    double time = 0;
    double lastTime = 0;
    double deltaTime = 0;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        deltaTime = time - lastTime;
        lastTime = time;

        std::cout << "FPS: " << 1/deltaTime << "\n";

        glClear(GL_COLOR_BUFFER_BIT);
        /* Render here */
        
        triangle.Draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
