#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "SceneObject.hpp"
#include "ShaderBuilder.hpp"
#include <string>
#include <set>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window, float deltaTime);

const unsigned int WIDTH = 800; 
const unsigned int HEIGHT = 600;

bool firstMouse = true;
float lastX = WIDTH/2;
float lastY = HEIGHT/2;

// Rotation Euler angles +X = Look Up; +Y = Look Left

Camera mainCamera = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
Camera freeCamera = Camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0));
Camera topDownCamera = Camera(glm::vec3(0, 5, 0), glm::vec3(90, 90, 0));

float randomFloat()
{
    return (float)(std::rand()) / (float)(RAND_MAX);
}
 
int randomInt(int a, int b)
{
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (std::rand() % (b - a));
}

float randomFloat(int a, int b)
{
    if (a > b)
        return randomFloat(b, a);
    if (a == b)
        return a;
 
    return (float)randomInt(a, b) + randomFloat();
}

int main(int argc, char *argv[])
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Graph 3D", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

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
    
    std::string var1 = "s";
    std::string var2 = "t";
    std::string equationX = "2*sin(s)*cos(t)"; //2*sin(s)*cos(t)
    std::string equationY = "2*sin(s)*sin(t)"; //2*sin(s)*sin(t)
    std::string equationZ = "2*cos(s)"; //2*cos(s)
    std::string timeString = "time";
    const int maxEquationSize = 256;
    int s_steps = 100;
    int t_steps = 100;
    float min_s, max_s, min_t, max_t;
    min_s = min_t = 0;
    max_s = M_PI;
    max_t = 2*M_PI;
    bool perfomanceCounter = true;
    
    for(int i = 1; i < argc; i++){
        std::string argvString = argv[i];
        if(argvString == "-x" && i < argc - 1){
            std::string value = argv[i+1];
            if(value.size() <= maxEquationSize){
                equationX = argv[i+1];
                continue;
            }
        }
        if(argvString == "-y" && i < argc - 1){
            std::string value = argv[i+1];
            if(value.size() <= maxEquationSize){
                equationY = argv[i+1];
                continue;
            }
        }
        if(argvString == "-z" && i < argc - 1){
            std::string value = argv[i+1];
            if(value.size() <= maxEquationSize){
                equationZ = argv[i+1];
                continue;
            }
        }
        if(argvString == "-t" && i < argc - 1){
            timeString = argv[i+1];
            continue;
        }
        if(argvString == "-v1" && i < argc - 1){
            var1 = argv[i+1];
            continue;
        }
        if(argvString == "-v2" && i < argc - 1){
            var2 = argv[i+1];
            continue;
        }
        if(argvString == "--min_v1" && i < argc - 1){
            try{
            min_s = std::stof(argv[i+1]);
            continue;
            } catch (const std::invalid_argument &e){
                std::cout << e.what() << " - Invalid value for argument min_v1\n";
            } catch (const std::out_of_range &e){
                std::cout << e.what() << " - Out of range value\n";
            }
        }
        if(argvString == "--max_v1" && i < argc - 1){
            try{
            max_s = std::stof(argv[i+1]);
            continue;
            }catch (const std::invalid_argument &e){
                std::cout << e.what() << " - Invalid value for argument max_v1\n";
            } catch (const std::out_of_range &e){
                std::cout << e.what() << " - Out of range value\n";
            }
        }
        if(argvString == "--min_v2" && i < argc - 1){
            try{
            min_t = std::stof(argv[i+1]);
            continue;
            }catch (const std::invalid_argument &e){
                std::cout << e.what() << " - Invalid value for argument min_v2\n";
            } catch (const std::out_of_range &e){
                std::cout << e.what() << " - Out of range value\n";
            }
        }
        if(argvString == "--max_v2" && i < argc - 1){
            try{
            max_t = std::stof(argv[i+1]);
            continue;
            }catch (const std::invalid_argument &e){
                std::cout << e.what() << " - Invalid value for argument max_v2\n";
            } catch (const std::out_of_range &e){
                std::cout << e.what() << " - Out of range value\n";
            }
        }
        if(argvString == "--steps_v1" && i < argc - 1){
            try{
            s_steps = std::stoi(argv[i+1]);
            continue;
            }catch (const std::invalid_argument &e){
                std::cout << e.what() << " - Invalid value for argument steps_v1\n";
            } catch (const std::out_of_range &e){
                std::cout << e.what() << " - Out of range value\n";
            }
        }
        if(argvString == "--steps_v2" && i < argc - 1){
            try{
            t_steps = std::stoi(argv[i+1]);
            continue;
            }catch (const std::invalid_argument &e){
                std::cout << e.what() << " - Invalid value for argument steps_v2\n";
            } catch (const std::out_of_range &e){
                std::cout << e.what() << " - Out of range value\n";
            }
        }
        if(argvString == "-c" && i < argc -1){
            std::string flag = argv[i+1];
            if(flag == "true" || flag == "1"){
                perfomanceCounter = true;
                continue;
            } else if(flag == "false" || flag == "0"){
                perfomanceCounter = false;
                continue;
            } 
        }
    }
    if(min_s > max_s){
        std::swap(min_s, max_s);
        std::cout << "Min Var1 is lesser than Max Var1 - Swapping values\n";
    }
    if(min_t > max_t){
        std::swap(min_t, max_t);
        std::cout << "Min Var2 is lesser than Max Var2 - Swapping values\n";
    }
    float s_stepSize = (max_s - min_s)/s_steps;
    float t_stepSize = (max_t - min_t)/t_steps;

    std::vector<GLfloat> parameters(2*(s_steps+1)*(t_steps+1));
    for(int i = 0; i < s_steps+1; i++) {
        for(int j = 0; j < t_steps+1; j++) {
            float s = min_s + s_stepSize*j;
            float t = min_t + t_stepSize*i;
            parameters[2*i*(s_steps+1)+2*j] = s;
            parameters[2*i*(s_steps+1)+2*j+1] = t;
        }
    }
    std::vector<GLuint> indices(2*s_steps*(t_steps+1) + 2*t_steps*(s_steps+1));
    int i = 0;
    for(int y = 0; y < t_steps+1; y++) {
        for(int x = 0; x < s_steps; x++) {
            indices[i++] = y * (s_steps+1) + x;
            indices[i++] = y * (s_steps+1) + x + 1;
        }
    }

    // Vertical grid lines
    for(int x = 0; x < s_steps+1; x++) {
        for(int y = 0; y < t_steps; y++) {
            indices[i++] = y * (t_steps+1) + x;
            indices[i++] = (y + 1) * (t_steps+1) + x;
        }
    }
    SceneObject graph = SceneObject(1);
    //std::cout << indices.size()*sizeof(GLuint) << std::endl;
    GLsizei parametersSize = parameters.size()*sizeof(GLfloat);
    GLsizei indicesSize = indices.size()*sizeof(GLuint);
    graph.BufferStorageEmpty(0, parametersSize + indicesSize);
    graph.BufferSubData(0, parametersSize, indices);
    graph.BufferSubData(0, 0, parameters);
    graph.AttachVertexBuffer(0, 0, 0, 2*sizeof(float));
    graph.AttachElementBuffer(0);
    graph.SetIndicesInfo(indices.size(), parametersSize);
    graph.SetAttribute(0, 0, 2, GL_FLOAT, GL_FALSE, 0);

    ShaderBuilder vertexShaderBuilder = ShaderBuilder(false);
    ShaderBuilder fragmentShaderBuilder = ShaderBuilder(false);
    ShaderObject graphVertexShader = vertexShaderBuilder.SetShaderType(GL_VERTEX_SHADER)
    .SetVersion(330)
    .AddInput(0, SH_VEC2, "params")
    .AddUniform(SH_MAT4, "model")
    .AddUniform(SH_MAT4, "view")
    .AddUniform(SH_MAT4, "projection")
    .AddUniform(SH_FLOAT, timeString)
    .SetMain("float " + var1 + "= params.x;"
    "float " + var2 + "= params.y;"
    "float x =" + equationX + ";"
    "float y =" + equationY + ";"
    "float z =" + equationZ + ";"
    + "gl_Position = projection*view*model*vec4(x, z, y, 1.0);").Build();
    ShaderObject graphFragmentShader = fragmentShaderBuilder.SetShaderType(GL_FRAGMENT_SHADER)
    .SetVersion(330)
    .AddOutput(SH_VEC4, "FragColor")
    .AddUniform(SH_FLOAT, "red")
    .SetMain("FragColor = vec4(red, 0.0, 0.0, 1.0);").Build();
    ShaderProgram shader = ShaderProgram(std::vector<ShaderObject>{graphVertexShader, graphFragmentShader});
    shader.SetFloat("red", 1.0f);

    graph.SetShader(shader);
    graph.UpdateProjection("projection", WIDTH, HEIGHT);

    glClearColor(0, 0, 0, 1);
    double time = 0;
    double lastTime = 0;
    double deltaTime = 0;
    double maxDeltaTime = 0;
    double minDeltaTime = 0;
    unsigned long ticks = 0;

    mainCamera = freeCamera;
    bool isFreeCamera = true;
    bool holdingCameraSwitchKey = false;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        deltaTime = time - lastTime;
        lastTime = time;
        if(perfomanceCounter){
            if(minDeltaTime == 0){
                minDeltaTime = deltaTime;
            }
            if(deltaTime < minDeltaTime){
                minDeltaTime = deltaTime;
            }
            if(deltaTime > maxDeltaTime){
                maxDeltaTime = deltaTime;
            }
            ticks++;
        }
        
        //processInput(window, deltaTime);
        //std::cout << "FPS: " << 1/deltaTime << "\n";
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            if(perfomanceCounter){
                std::cout << "Min Delta Time: " << minDeltaTime << "(s) / " << 1000*minDeltaTime << "(ms)\n";
                std::cout << "Max Delta Time: " << maxDeltaTime << "(s) / " << 1000*maxDeltaTime << "(ms)\n";
                std::cout << "Ticks: " << ticks << "; Ticks/Sec: " << ticks/time << "\n";
            }
            glfwSetWindowShouldClose(window, true);
        }

        float cameraSpeed = static_cast<float>(2 * deltaTime);
        if(isFreeCamera){
            //std::cout << freeCamera.front.x << ", " << freeCamera.front.y << ", " << freeCamera.front.z << std::endl;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                freeCamera.transform.position += cameraSpeed * freeCamera.transform.Forward();
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                freeCamera.transform.position -= cameraSpeed * freeCamera.transform.Forward();
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                freeCamera.transform.position -= freeCamera.transform.Right() * cameraSpeed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                freeCamera.transform.position += freeCamera.transform.Right() * cameraSpeed;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                freeCamera.transform.position += cameraSpeed * freeCamera.transform.Up();

            mainCamera = freeCamera;
        } else {
            mainCamera = topDownCamera;
        }
        
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !holdingCameraSwitchKey){
            holdingCameraSwitchKey = true;
            isFreeCamera = !isFreeCamera;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && holdingCameraSwitchKey){
            holdingCameraSwitchKey = false;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        /* Render here */

        graph.transform.eulerAngles(glm::vec3(0, 30*time, 0));
        shader.SetFloat("time", time);
        graph.UpdateModel("model");
        graph.UpdateView("view", mainCamera);
        graph.DrawLines();

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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    freeCamera.ProcessMouseMovement(xoffset, yoffset);
}