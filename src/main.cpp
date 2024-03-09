#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "SceneObject.hpp"
#include <string>
#include <set>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime);

const unsigned int WIDTH = 800; 
const unsigned int HEIGHT = 600;



int main(void)
{   
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
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
        std::cout << "Direct access extension suported\n\n";
    
    int N = 64;
    float graphSemiWidth = 5;
    
    float vertices[2*(N+1)*(N+1)];
    
    for(int i = 0; i < N+1; i++) {
        for(int j = 0; j < N+1; j++) {
            float x = (2*graphSemiWidth*(j - (N/2))) / N;
            float y = (2*graphSemiWidth*(i - (N/2))) / N;
            vertices[2*i*(N+1)+2*j] = x;
            vertices[2*i*(N+1)+2*j+1] = y;
        }
    }
    GLushort indices[2*N*(N+1)*2];
    int i = 0;
    for(int y = 0; y < N+1; y++) {
        for(int x = 0; x < N; x++) {
            indices[i++] = y * (N+1) + x;
            indices[i++] = y * (N+1) + x + 1;
        }
    }

    // Vertical grid lines
    for(int x = 0; x < N+1; x++) {
        for(int y = 0; y < N; y++) {
            indices[i++] = y * (N+1) + x;
            indices[i++] = (y + 1) * (N+1) + x;
        }
    }
    SceneObject triangle = SceneObject(1);
    triangle.StartImmutableBufferStorage(0, vertices, sizeof(vertices));
    triangle.StartElementBufferStorage(indices, sizeof(indices));
    triangle.AttachVertexBuffer(0, 0, 0, 2*sizeof(float));
    triangle.AttachElementBuffer();
    triangle.SetAttribute(0, 0, 2, GL_FLOAT, GL_FALSE, 0);

    std::string triangleVertexShaderPath = "../resources/basic.vert";
    std::string triangleFragmentShaderPath = "../resources/basic.frag";
    ShaderObject triangleVertexShader = ShaderObject(GL_VERTEX_SHADER, triangleVertexShaderPath, true);
    ShaderObject triangleFragmentShader = ShaderObject(GL_FRAGMENT_SHADER, triangleFragmentShaderPath, true);
    ShaderProgram shader = ShaderProgram();
    shader.AttachShaderObject(triangleVertexShader);
    shader.AttachShaderObject(triangleFragmentShader);
    shader.Link();
    shader.DetachShaderObject(triangleVertexShader);
    shader.DetachShaderObject(triangleFragmentShader);
    shader.SetFloat("red", 0.5f);

    triangle.SetShader(shader);
    triangle.UpdateProjection("projection", WIDTH, HEIGHT);

    glClearColor(0, 0, 0, 1);
    double time = 0;
    double lastTime = 0;
    double deltaTime = 0;
    glm::vec3 cameraPos = glm::vec3(0, 0, -3);
    glm::vec3 cameraFront = glm::vec3(0, 0, 1);
    glm::vec3 cameraUp = glm::vec3(0, 1, 0);

    Camera mainCamera = Camera();
    mainCamera.transform.position = cameraPos;
    mainCamera.front = cameraFront;
    mainCamera.up = cameraUp;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        deltaTime = time - lastTime;
        lastTime = time;

        //processInput(window, deltaTime);
        //std::cout << "FPS: " << 1/deltaTime << "\n";
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float cameraSpeed = static_cast<float>(2.5 * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mainCamera.transform.position += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mainCamera.transform.position -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mainCamera.transform.position -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mainCamera.transform.position += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        glClear(GL_COLOR_BUFFER_BIT);
        /* Render here */
        triangle.UpdateModel("model");
        triangle.UpdateView("view", mainCamera);
        triangle.DrawLines();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
void processInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
