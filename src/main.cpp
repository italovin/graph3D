#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Base.hpp"
#include "Components.hpp"
#include "GLApiVersions.hpp"
#include "Material.hpp"
#include "ShaderCode.hpp"
#include "Input.hpp"
#include "Renderer.hpp"
#include "ShaderTypes.hpp"
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

int main(int argc, char *argv[])
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    ///glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
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
    std::cout << glfwGetWindowAttrib(window.GetHandle(), GLFW_CONTEXT_CREATION_API) << std::endl;

    GLint size;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
    std::cout << "GL_MAX_UNIFORM_BLOCK_SIZE is " << size << " bytes." << std::endl;
    GLint size2;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &size2);
    std::cout << "GL_MAX_ARRAY_TEXTURE_LAYERS is " << size2 << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::cout << glGetString(GL_VENDOR) << std::endl;

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

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugCallback, nullptr);

    const int glslVersion = window.GetGLSLVersion();

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

    std::vector<GLfloat> parameters;
    parameters.reserve(2*(s_steps+1)*(t_steps+1));
    for(int i = 0; i < s_steps+1; i++) {
        for(int j = 0; j < t_steps+1; j++) {
            float s = min_s + s_stepSize*j;
            float t = min_t + t_stepSize*i;
            parameters.push_back(s);
            parameters.push_back(t);
        }
    }
    std::vector<GLuint> indices;
    indices.reserve(2*s_steps*(t_steps+1) + 2*t_steps*(s_steps+1));
    int i = 0;
    for(int y = 0; y < t_steps+1; y++) {
        for(int x = 0; x < s_steps; x++) {
            indices.push_back(y * (s_steps+1) + x);
            indices.push_back(y * (s_steps+1) + x + 1);
        }
    }

    // Vertical grid lines
    for(int x = 0; x < s_steps+1; x++) {
        for(int y = 0; y < t_steps; y++) {
            indices.push_back(y * (t_steps+1) + x);
            indices.push_back((y + 1) * (t_steps+1) + x);
        }
    }
    // Useful for indexing objects on shader
    std::string objIDString;
    if(GLApiVersionStruct::GetVersionFromInteger(glslVersion) < GLApiVersion::V460){
        objIDString = "gl_BaseInstanceARB + gl_InstanceID";
    } else {
        objIDString = "gl_BaseInstance + gl_InstanceID";
    }
    ////

    Ref<Mesh> graphMesh = CreateRef<Mesh>();
    graphMesh->PushAttribute("params", 0, MeshAttributeFormat::Vec2, false, parameters);
    graphMesh->SetIndices(indices, MeshTopology::Lines);

    ShaderCode graphShaderCode = ShaderCode();
    graphShaderCode.SetVersion(glslVersion);
    if(GLApiVersionStruct::GetVersionFromInteger(glslVersion) < GLApiVersion::V460)
        graphShaderCode.AddExtension("GL_ARB_shader_draw_parameters");
    graphShaderCode.SetStageToPipeline(ShaderStage::Vertex, true);
    graphShaderCode.AddVertexAttribute("params", ShaderDataType::Float2, 0);
    graphShaderCode.CreateUniformBlock(ShaderStage::Vertex, "mvpsUBO", "mat4 mvps[512];");
    graphShaderCode.SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", "mvps");
    graphShaderCode.AddUniform(ShaderStage::Vertex, timeString, ShaderDataType::Float);
    graphShaderCode.SetMain(ShaderStage::Vertex,
    "int objID = " + objIDString + ";"
    "float " + var1 + "= params.x;"
    "float " + var2 + "= params.y;"
    "float x =" + equationX + ";"
    "float y =" + equationY + ";"
    "float z =" + equationZ + ";"
    + "gl_Position = mvps[objID]*vec4(x, z, y, 1.0);");
    graphShaderCode.SetStageToPipeline(ShaderStage::Fragment, true);
    graphShaderCode.AddOutput(ShaderStage::Fragment, "FragColor", ShaderDataType::Float4);
    graphShaderCode.SetMain(ShaderStage::Fragment, "FragColor = vec4(1.0, 0.0, 0.0, 1.0);");
    Ref<Material> graphMaterial = CreateRef<Material>(graphShaderCode);


    Ref<Mesh> mesh = CreateRef<Mesh>();
    Ref<Mesh> mesh2 = CreateRef<Mesh>();

    std::vector<float> quad = {-0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.5f,  0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f};
    std::vector<unsigned short> quadIndices = {
        0, 1, 2, 2, 3, 0
    };
    std::vector<unsigned char> color1 = {255, 0, 0, 255,
    255, 0, 0, 255,
    255, 0, 0, 255,
    255, 0, 0, 255};
    std::vector<unsigned char> color2 = {0, 0, 255, 255,
    0, 0, 255, 255,
    0, 0, 255, 255};
    std::vector<float> triangle = {-0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f};
    mesh->PushAttribute("pos", 0, MeshAttributeFormat::Vec3, false, quad);
    //mesh->PushAttribute("color", 1, MeshAttributeFormat::Vec4, true, color1);
    mesh->SetIndices(quadIndices, MeshTopology::Triangles);
    mesh2->PushAttribute("pos", 0, MeshAttributeFormat::Vec3, false, triangle);
    //mesh2->PushAttribute("color", 1, MeshAttributeFormat::Vec4, true, color2);
    mesh2->SetIndices(std::vector<unsigned short>{0, 1, 2}, MeshTopology::Triangles);

    ShaderCode shaderCodeTest = ShaderCode();
    shaderCodeTest.SetVersion(glslVersion);
    if(GLApiVersionStruct::GetVersionFromInteger(glslVersion) < GLApiVersion::V460)
        shaderCodeTest.AddExtension("GL_ARB_shader_draw_parameters");
    shaderCodeTest.SetStageToPipeline(ShaderStage::Vertex, true);
    shaderCodeTest.AddVertexAttribute("aPos", ShaderDataType::Float3, 0);
    shaderCodeTest.AddVertexAttribute("aColor", ShaderDataType::Float4, 1);
    shaderCodeTest.AddOutput(ShaderStage::Vertex, "colorOut", ShaderDataType::Float4);
    shaderCodeTest.AddOutput(ShaderStage::Vertex, "matID", ShaderDataType::Int);
    shaderCodeTest.CreateUniformBlock(ShaderStage::Vertex, "mvpsUBO", "mat4 mvps[512];");
    shaderCodeTest.SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", "mvps");
    shaderCodeTest.SetMain(ShaderStage::Vertex,
    "int objID = " + objIDString + ";\n"
    "colorOut = aColor;\n"
    "matID = objID;\n"
    "gl_Position = mvps[objID]*vec4(aPos, 1.0);");
    shaderCodeTest.SetStageToPipeline(ShaderStage::Fragment, true);
    shaderCodeTest.DefineMaterialParametersStruct(ShaderStage::Fragment, "Material");
    shaderCodeTest.AddMaterialParameterToStruct("Material", ShaderStage::Fragment, "albedo", ShaderDataType::Float4);
    shaderCodeTest.UpdateMaterialParameterUniformBlock(ShaderStage::Fragment, "matUBO", "Material materials[512];");
    shaderCodeTest.SetBindingPurpose(ShaderStage::Fragment, "matUBO", "materials");
    shaderCodeTest.AddOutput(ShaderStage::Fragment, "FragColor", ShaderDataType::Float4);
    shaderCodeTest.SetMain(ShaderStage::Fragment, "FragColor = materials[matID].albedo;");
    Ref<Material> materialTest = CreateRef<Material>(shaderCodeTest);
    Ref<Material> materialTestGreen = CreateRef<Material>(shaderCodeTest);
    Ref<Material> materialTestRed = CreateRef<Material>(shaderCodeTest);
    materialTest->SetParameterVector4("albedo", glm::vec4(0, 0, 1, 1));
    materialTestGreen->SetParameterVector4("albedo", glm::vec4(0, 1, 0, 1));
    materialTestRed->SetParameterVector4("albedo", glm::vec4(1, 0, 0, 1));

    std::filesystem::path modelsDirectory;
    if(std::filesystem::exists(std::filesystem::path("../resources/modelos"))){
        modelsDirectory = std::filesystem::path("../resources/modelos");
    } else if(std::filesystem::exists(std::filesystem::path("./resources/modelos"))){
        modelsDirectory = std::filesystem::path("./resources/modelos");
    } else {
        std::cout << "Insira manualmente o diretório base dos modelos:";
        std::cin >> modelsDirectory;
    }
    if(!std::filesystem::exists(modelsDirectory)){
        std::cout << "Diretório dos modelos inválido\n";
        return -1;
    }

    const std::string modelsVerticesKey = "vertices";
    const std::string modelsIndicessKey = "indices";

    std::ifstream model1Stream(modelsDirectory/"ball.json");
    if(!model1Stream || !nlohmann::json::accept(model1Stream)){
        std::cout << "JSON do model 1 não existe ou é inválido\n";
        return -1;
    }
    model1Stream.seekg(0);

    nlohmann::json model1Json = nlohmann::json::parse(model1Stream);
    std::vector<float> model1Vertices = model1Json[modelsVerticesKey].get<std::vector<float>>();
    std::vector<unsigned short> model1Indices = model1Json[modelsIndicessKey].get<std::vector<unsigned short>>();
    Ref<Mesh> model1Mesh = CreateRef<Mesh>();
    model1Mesh->PushAttribute("pos", 0, MeshAttributeFormat::Vec3, false, model1Vertices);
    model1Mesh->SetIndices(model1Indices, MeshTopology::Triangles);
    Ref<Material> model1Material = CreateRef<Material>(shaderCodeTest);
    model1Material->SetParameterVector4("albedo", glm::vec4(1, 0, 0, 1));

    std::ifstream model2Stream(modelsDirectory/"cone1.json");
    if(!model2Stream || !nlohmann::json::accept(model2Stream)){
        std::cout << "JSON do model 2 não existe ou é inválido\n";
        return -1;
    }
    model2Stream.seekg(0);

    nlohmann::json model2Json = nlohmann::json::parse(model2Stream);
    std::vector<float> model2Vertices = model2Json[modelsVerticesKey].get<std::vector<float>>();
    std::vector<unsigned short> model2Indices = model2Json[modelsIndicessKey].get<std::vector<unsigned short>>();
    Ref<Mesh> model2Mesh = CreateRef<Mesh>();
    model2Mesh->PushAttribute("pos", 0, MeshAttributeFormat::Vec3, false, model2Vertices);
    model2Mesh->SetIndices(model2Indices, MeshTopology::Triangles);
    Ref<Material> model2Material = CreateRef<Material>(shaderCodeTest);
    model2Material->SetParameterVector4("albedo", glm::vec4(0, 1, 0, 1));

    std::ifstream model3Stream(modelsDirectory/"cylinder.json");
    if(!model3Stream || !nlohmann::json::accept(model3Stream)){
        std::cout << "JSON do model 3 não existe ou é inválido\n";
        return -1;
    }
    model3Stream.seekg(0);

    nlohmann::json model3Json = nlohmann::json::parse(model3Stream);
    std::vector<float> model3Vertices = model3Json[modelsVerticesKey].get<std::vector<float>>();
    std::vector<unsigned short> model3Indices = model3Json[modelsIndicessKey].get<std::vector<unsigned short>>();
    Ref<Mesh> model3Mesh = CreateRef<Mesh>();
    model3Mesh->PushAttribute("pos", 0, MeshAttributeFormat::Vec3, false, model3Vertices);
    model3Mesh->SetIndices(model3Indices, MeshTopology::Triangles);
    Ref<Material> model3Material = CreateRef<Material>(shaderCodeTest);
    model3Material->SetParameterVector4("albedo", glm::vec4(0, 0, 1, 1));

    Scene mainScene;
    Entity quad1 = mainScene.CreateEntity();
    Entity tri1 = mainScene.CreateEntity();
    Entity quad2 = mainScene.CreateEntity();
    Entity graph = mainScene.CreateEntity();
    Entity model1 = mainScene.CreateEntity();
    Entity model2 = mainScene.CreateEntity();
    Entity model3 = mainScene.CreateEntity();
    Entity mainCamera = mainScene.CreateEntity();
    // Rotation Euler angles +X = Look Down; +Y = Look Right
    // Left handed system is ok with rotations: Positive rotations are clockwise and z+ points into screen
    TransformComponent freeCameraTransform;
    freeCameraTransform.position = glm::vec3(0, 0, 3);
    freeCameraTransform.eulerAngles(glm::vec3(0, 180, 0));
    TransformComponent topDownCameraTransform;
    topDownCameraTransform.position = glm::vec3(0, 5, 0);
    topDownCameraTransform.eulerAngles(glm::vec3(90, 0, 0));

    // quad1.AddComponent<MeshRendererComponent>(mesh, materialTest);
    // quad1.GetComponent<TransformComponent>().position = glm::vec3(0, 0, 0);
    //
    // tri1.AddComponent<MeshRendererComponent>(mesh2, materialTestGreen);
    // tri1.GetComponent<TransformComponent>().position = glm::vec3(0, 1, 0);
    //
    // quad2.AddComponent<MeshRendererComponent>(mesh, materialTestRed);
    // quad2.GetComponent<TransformComponent>().position = glm::vec3(0, -1, 0);
    //
    // graph.AddComponent<MeshRendererComponent>(graphMesh, graphMaterial);
    // graph.GetComponent<TransformComponent>().position = glm::vec3(0, 0, 0);

    model1.AddComponent<MeshRendererComponent>(model1Mesh, model1Material);
    model1.GetComponent<TransformComponent>().position = glm::vec3(-2, 0, 0);
    model2.AddComponent<MeshRendererComponent>(model2Mesh, model2Material);
    model2.GetComponent<TransformComponent>().position = glm::vec3(2, 0, 0);
    model3.AddComponent<MeshRendererComponent>(model3Mesh, model3Material);
    model3.GetComponent<TransformComponent>().position = glm::vec3(0, 0, 0);
    model3.transform.scale = glm::vec3(0.2f, 0.2f, 0.2f);

    mainCamera.AddComponent<CameraComponent>().isMain = true;

    Renderer mainRenderer = Renderer();

    mainRenderer.SetMainWindow(std::addressof(window));
    mainRenderer.SetAPIVersion(GLApiVersionStruct::GetVersionFromInteger(glslVersion));
    double initialRendererTime = glfwGetTime();
    mainRenderer.Start(mainScene.registry);
    double prepareTime = glfwGetTime() - initialRendererTime;
    std::cout << "Time to prepare for meshes for grouping: " << 1000*prepareTime << " (ms)\n";
    std::cout << "Computed draw groups: " << mainRenderer.GetDrawGroupsCount() << "\n";

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    double time = 0;
    double lastTime = 0;
    double deltaTime = 0;
    double maxDeltaTime = 0;
    double minDeltaTime = 0;
    unsigned long ticks = 0;

    mainCamera.transform = freeCameraTransform;
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
            glm::vec3 up = freeCameraTransform.Up();
            glm::vec3 right = freeCameraTransform.Right();
            glm::vec3 forward = freeCameraTransform.Forward();
            float cameraSpeed = static_cast<float>(1.5 * deltaTime);

            float horizontalKeyboard = Input::GetAxis(KeyboardAxis::Horizontal);
            float verticalKeyboard = Input::GetAxis(KeyboardAxis::Vertical);
            if (Input::GetKeyHeld(GLFW_KEY_SPACE)){
                freeCameraTransform.position += cameraSpeed * up;
            }
            int jid = GLFW_JOYSTICK_1;
            float horizontalJoystick = Input::GetJoystickAxisLeftX(jid);
            float verticalJoystick = Input::GetJoystickAxisLeftY(jid);
            float horizontal = glm::abs(horizontalKeyboard) > glm::abs(horizontalJoystick) ? horizontalKeyboard:horizontalJoystick;
            float vertical = glm::abs(verticalKeyboard) > glm::abs(verticalJoystick) ? verticalKeyboard:verticalJoystick;
            freeCameraTransform.position += cameraSpeed * right * horizontal;
            freeCameraTransform.position += cameraSpeed * forward * vertical;
            float factor = 0.01f;
            glm::quat qPitch = glm::angleAxis(-factor*Input::GetMouseDeltaY(), glm::vec3(1, 0, 0));
            glm::quat qYaw = glm::angleAxis(factor*Input::GetMouseDeltaX(), glm::vec3(0, 1, 0));
            freeCameraTransform.rotation =  qYaw * freeCameraTransform.rotation;
            freeCameraTransform.rotation = freeCameraTransform.rotation * qPitch;
            mainCamera.transform = freeCameraTransform;
        } else {
            mainCamera.transform = topDownCameraTransform;
        }
        if (Input::GetKeyDown(GLFW_KEY_T)){
            isFreeCamera = !isFreeCamera;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* Render here */

        //quad1.transform.eulerAngles(glm::vec3(0, 30*time, 0));

        //graph.GetComponent<MeshRendererComponent>().material->SetGlobalParameterFloat(timeString, time);
        //graph.transform.eulerAngles(glm::vec3(0, -30*time, 0));
        model2.transform.eulerAngles(glm::vec3(30*time, -30*time, 0));
        model3.transform.eulerAngles(glm::vec3(-30*time, 30*time, 0));

        mainRenderer.Update(mainScene.registry, deltaTime);
        /* Swap front and back buffers */
        glfwSwapBuffers(window.GetHandle());

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwDestroyWindow(window.GetHandle());
    glfwTerminate();
    return 0;
}

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
        return "UNDEFINED SOURCE";
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
        return "UNDEFINED TYPE";
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
        return "UNDEFINED SEVERITY";
	}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}
