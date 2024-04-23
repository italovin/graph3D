#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "SceneObject.hpp"
#include "ShaderBuilder.hpp"
#include "Input.hpp"
#include "Renderer.hpp"
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    const int WIDTH = 800; 
    const int HEIGHT = 600;

    /* Create a windowed mode window and its OpenGL context */
    Window window;

    if (!window.Create("Graph 3D", WIDTH, HEIGHT))
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    window.MakeContextCurrent();
    Input::RegisterWindow(window);

    std::cout << glGetString(GL_VERSION) << std::endl;

    GLint no_of_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);
    int minorVersion = 0;
    int majorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    window.SetContextVersion(majorVersion, minorVersion, 0);

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

    int glslVersion = window.GetGLSLVersion();
    ShaderBuilder vertexShaderBuilder = ShaderBuilder(false);
    ShaderBuilder fragmentShaderBuilder = ShaderBuilder(false);
    ShaderObject graphVertexShader = vertexShaderBuilder.SetShaderType(GL_VERTEX_SHADER)
    .SetVersion(glslVersion)
    .AddInput(0, SH_VEC2, "params")
    .AddUniform(SH_MAT4, "mvp")
    .AddUniform(SH_FLOAT, timeString)
    .SetMain("float " + var1 + "= params.x;"
    "float " + var2 + "= params.y;"
    "float x =" + equationX + ";"
    "float y =" + equationY + ";"
    "float z =" + equationZ + ";"
    + "gl_Position = mvp*vec4(x, z, y, 1.0);").Build();
    ShaderObject graphFragmentShader = fragmentShaderBuilder.SetShaderType(GL_FRAGMENT_SHADER)
    .SetVersion(glslVersion)
    .AddOutput(SH_VEC4, "FragColor")
    .AddUniform(SH_FLOAT, "red")
    .SetMain("FragColor = vec4(red, 0.0, 0.0, 1.0);").Build();
    ShaderProgram shader = ShaderProgram(std::vector<ShaderObject>{graphVertexShader, graphFragmentShader});
    shader.SetFloat("red", 1.0f);

    graph.SetShader(shader);
    glm::mat4 projection = window.GetProjectionMatrix();

    Mesh mesh = Mesh();
    std::vector<float> triangle = {-1, -1, 0,
    1, -1, 0,
    0, 1, 0};
    std::vector<unsigned int> triangleIndices = {
        0, 1, 2
    };
    mesh.PushAttribute("positions", 0, triangle, 3, false);
    mesh.SetIndices(triangleIndices, Triangle);
    ShaderBuilder testVBuilder = ShaderBuilder(false);
    ShaderBuilder testFBuilder = ShaderBuilder(false);
    ShaderObject testV = testVBuilder.SetShaderType(GL_VERTEX_SHADER)
    .SetVersion(glslVersion)
    .AddInput(0, SH_VEC3, "aPos")
    .SetMain("gl_Position = vec4(aPos, 1.0);").Build();
    ShaderObject testF = testFBuilder.SetShaderType(GL_FRAGMENT_SHADER)
    .SetVersion(glslVersion)
    .AddOutput(SH_VEC4, "FragColor")
    .SetMain("FragColor = vec4(1.0);").Build();
    ShaderProgram testShader = ShaderProgram(std::vector<ShaderObject>{testV, testF});
    MeshRenderer meshRenderer = MeshRenderer();
    meshRenderer.SetMesh(mesh);
    meshRenderer.SetShader(testShader);
    Renderer mainRenderer = Renderer();
    std::vector<MeshRenderer> meshRenderers = { meshRenderer };
    mainRenderer.Prepare(meshRenderers);

    // Rotation Euler angles +X = Look Down; +Y = Look Right
    // Left handed system is ok with rotations: Positive rotations are clockwise and z+ points into screen
    Camera mainCamera = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
    Camera freeCamera = Camera(glm::vec3(0, 0, 3), glm::vec3(0, 180, 0));
    Camera topDownCamera = Camera(glm::vec3(0, 5, 0), glm::vec3(90, 0, 0));

    glClearColor(0, 0, 0, 1);
    double time = 0;
    double lastTime = 0;
    double deltaTime = 0;
    double maxDeltaTime = 0;
    double minDeltaTime = 0;
    unsigned long ticks = 0;

    mainCamera = freeCamera;
    bool isFreeCamera = true;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.GetHandle()))
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
        //Input
        Input::Update(window);
        ///////

        if (Input::GetKeyDown(GLFW_KEY_ESCAPE)){
            if(perfomanceCounter){
                std::cout << "Min Delta Time: " << minDeltaTime << "(s) / " << 1000*minDeltaTime << "(ms)\n";
                std::cout << "Max Delta Time: " << maxDeltaTime << "(s) / " << 1000*maxDeltaTime << "(ms)\n";
                std::cout << "Ticks: " << ticks << "; Ticks/Sec: " << ticks/time << "\n";
            }
            glfwSetWindowShouldClose(window.GetHandle(), true);
        }

        
        if(isFreeCamera){
            glm::vec3 up = freeCamera.transform.Up();
            glm::vec3 right = freeCamera.transform.Right();
            glm::vec3 forward = freeCamera.transform.Forward();
            float cameraSpeed = static_cast<float>(1.5 * deltaTime);
            
            float horizontalKeyboard = Input::GetAxis(KEYB_AXIS_HORIZONTAL);
            float verticalKeyboard = Input::GetAxis(KEYB_AXIS_VERTICAL);
            if (Input::GetKeyHeld(GLFW_KEY_SPACE)){
                freeCamera.transform.position += cameraSpeed * up;
            }
            int jid = GLFW_JOYSTICK_1;
            float horizontalJoystick = Input::GetJoystickAxisLeftX(jid);
            float verticalJoystick = Input::GetJoystickAxisLeftY(jid);
            float horizontal = glm::abs(horizontalKeyboard) > glm::abs(horizontalJoystick) ? horizontalKeyboard:horizontalJoystick;
            float vertical = glm::abs(verticalKeyboard) > glm::abs(verticalJoystick) ? verticalKeyboard:verticalJoystick;
            freeCamera.transform.position += cameraSpeed * right * horizontal;
            freeCamera.transform.position += cameraSpeed * forward * vertical;
            mainCamera = freeCamera;
            float factor = 0.01f;
            glm::quat qPitch = glm::angleAxis(-factor*Input::GetMouseDeltaY(), glm::vec3(1, 0, 0));
            glm::quat qYaw = glm::angleAxis(factor*Input::GetMouseDeltaX(), glm::vec3(0, 1, 0));
            freeCamera.transform.rotation =  qYaw * freeCamera.transform.rotation;
            freeCamera.transform.rotation = freeCamera.transform.rotation * qPitch;
        } else {
            mainCamera = topDownCamera;
        }
        if (Input::GetKeyDown(GLFW_KEY_T)){
            isFreeCamera = !isFreeCamera;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        /* Render here */

        graph.transform.eulerAngles(glm::vec3(0, 30*time, 0));
        
        shader.SetFloat("time", time);
        glm::mat4 model = graph.GetModelMatrix();
        glm::mat4 view = mainCamera.GetViewMatrix();
        glm::mat4 mvp = projection*view*model;
        graph.Shader().SetMat4Float("mvp", mvp);
        graph.DrawLines();
        mainRenderer.Draw();
        /* Swap front and back buffers */
        glfwSwapBuffers(window.GetHandle());

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}