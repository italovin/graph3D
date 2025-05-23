#include <GL/glew.h>
#include <SDL3/SDL.h>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "Base.hpp"
#include "Renderer.hpp"
#include "RenderCapabilities.hpp"
#include "ShaderStandard.hpp"
#include "ShaderCode.hpp"
#include "Input.hpp"
#include "Model.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <tbb/parallel_for.h>

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

int main(int argc, const char *argv[])
{
    setlocale(LC_ALL, ".UTF-8");

    //// SDL
    // Inicializa a SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return -1;
    }
    // Define configurações do OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // Versão principal do OpenGL (exemplo: 3.3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // Perfil core
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_SetRelativeMouseMode(SDL_TRUE);

    const int WIDTH = 800;
    const int HEIGHT = 600;

    /* Create a windowed mode window and its OpenGL context */
    Window window;

    if (!window.Create(std::string("Graph 3D"), WIDTH, HEIGHT))
    {
        std::cerr << "Erro ao criar janela: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    /* Make the window's context current */
    if (!window.MakeContextCurrent()) {
        std::cerr << "Erro ao criar contexto OpenGL: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window.GetHandle());
        SDL_Quit();
        return -1;
    }

    // Initialize the renderer API info
    window.SetContextVersion(RenderCapabilities::GetMajorVersion(), RenderCapabilities::GetMinorVersion(), 0);

    // Ativa sincronização vertical (VSync)
    if (!SDL_GL_SetSwapInterval(-1)) {
        if(!SDL_GL_SetSwapInterval(1))
            std::cerr << "Aviso: Não foi possível ativar VSync: " << SDL_GetError() << std::endl;
    }

    RenderCapabilities::Initialize();
    fmt::print("GL_MAX_UNIFORM_BLOCK_SIZE is {0} bytes\n", RenderCapabilities::GetMaxUBOSize());
    fmt::print("GL_MAX_ARRAY_TEXTURE_LAYERS is {0}\n", RenderCapabilities::GetMaxTextureArrayLayers());
    fmt::print("GL_MAX_TEXTURE_UNITS is {0}\n", RenderCapabilities::GetMaxTextureImageUnits());
    fmt::print("GL_MAX_VERTEX_ATTRIBS is {0}\n", RenderCapabilities::GetMaxVertexAttributes());
    fmt::print("GL_MAX_VERTEX_OUTPUT_COMPONENTS is {0}\n", RenderCapabilities::GetMaxVertexOutputComponents());
    fmt::print("GL_MAX_UNIFORM_BUFFER_BINDINGS is {0}\n", RenderCapabilities::GetMaxUBOBindings());
    fmt::println(RenderCapabilities::GetVersionString());
    fmt::println(RenderCapabilities::GetVendorString());

    /// GLEW
    // Init GLEW - GL extensions loader
    {
        GLenum err=glewInit();
        if(err != GLEW_OK){
            std::cout << "Failed to start GLEW" << std::endl;
            return -1;
        }
    }

    const int glslVersion = RenderCapabilities::GetGLSLVersion();

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
        if(argvString == "-d"){
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(GLDebugCallback, nullptr);
            continue;
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
    float graphTotalSize = sizeof(GLfloat)*(2*(s_steps+1)*(t_steps+1)) + sizeof(GLuint)*(2*s_steps*(t_steps+1) + 2*t_steps*(s_steps+1));
    fmt::print("Graph parameter S resolution: {0}\n", s_stepSize);
    fmt::print("Graph parameter T resolution: {0}\n", t_stepSize);
    fmt::print("Graph total size (vertices + indices): {0} KB / {1} MB\n", graphTotalSize/1024, graphTotalSize/(1024*1024));

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
    if(RenderCapabilities::GetAPIVersion() < GLApiVersion::V460){
        objIDString = "gl_DrawIDARB + gl_BaseInstanceARB + gl_InstanceID";
    } else {
        objIDString = "gl_BaseInstance + gl_InstanceID";
    }
    ////

    Ref<Mesh> graphMesh = CreateRef<Mesh>();
    graphMesh->PushAttribute("params", MeshAttributeFormat::Vec2, false, parameters);
    graphMesh->SetIndices(indices, MeshTopology::Lines);

    Ref<ShaderCode> graphShaderCode = CreateRef<ShaderCode>();
    graphShaderCode->SetVersion(glslVersion);
    if(RenderCapabilities::GetAPIVersion() < GLApiVersion::V460)
        graphShaderCode->AddExtension("GL_ARB_shader_draw_parameters");
    graphShaderCode->SetStageToPipeline(ShaderStage::Vertex, true);
    graphShaderCode->AddVertexAttribute("params", ShaderDataType::Float2, 0);
    graphShaderCode->CreateUniformBlock(ShaderStage::Vertex, "mvpsUBO", "mat4 mvps[512];");
    graphShaderCode->SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", "mvps");
    graphShaderCode->AddUniform(ShaderStage::Vertex, timeString, ShaderDataType::Float);
    graphShaderCode->SetMain(ShaderStage::Vertex,
    "int objID = " + objIDString + ";"
    "float " + var1 + "= params.x;"
    "float " + var2 + "= params.y;"
    "float x =" + equationX + ";"
    "float y =" + equationY + ";"
    "float z =" + equationZ + ";"
    + "gl_Position = mvps[objID]*vec4(x, z, y, 1.0);");
    graphShaderCode->SetStageToPipeline(ShaderStage::Fragment, true);
    graphShaderCode->AddOutput(ShaderStage::Fragment, "FragColor", ShaderDataType::Float4);
    graphShaderCode->SetMain(ShaderStage::Fragment, "FragColor = vec4(1.0, 0.0, 0.0, 1.0);");
    Ref<ShaderStandard> shaderStandard = CreateRef<ShaderStandard>();
    Ref<Shader> tempShader = CreateRef<Shader>();
    Ref<Material> graphMaterial = CreateRef<Material>(tempShader);

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
    mesh->PushAttributePosition(quad);
    mesh->PushAttributeTexCoord0(std::vector<float>{0,0, 1,0, 1,1, 0,1});
    mesh->SetIndices(quadIndices, MeshTopology::Triangles);
    mesh2->PushAttributePosition(triangle);
    mesh2->PushAttributeTexCoord0(std::vector<float>{0,0, 1,0, 0.5f,1});
    mesh2->SetIndices(std::vector<unsigned short>{0, 1, 2}, MeshTopology::Triangles);

    Ref<ShaderCode> shaderCodeTest = CreateRef<ShaderCode>();
    shaderCodeTest->SetVersion(glslVersion);
    if(RenderCapabilities::GetAPIVersion() < GLApiVersion::V460)
        shaderCodeTest->AddExtension("GL_ARB_shader_draw_parameters");
    shaderCodeTest->SetStageToPipeline(ShaderStage::Vertex, true);
    shaderCodeTest->AddVertexAttribute("aPos", ShaderDataType::Float3, 0);
    shaderCodeTest->AddVertexAttribute("aColor", ShaderDataType::Float4, 1);
    shaderCodeTest->AddVertexAttribute("aTexCoord", ShaderDataType::Float2, 2);
    shaderCodeTest->AddOutput(ShaderStage::Vertex, "colorOut", ShaderDataType::Float4);
    shaderCodeTest->AddOutput(ShaderStage::Vertex, "texCoordOut", ShaderDataType::Float2);
    shaderCodeTest->AddOutput(ShaderStage::Vertex, "matID", ShaderDataType::Int);
    shaderCodeTest->CreateUniformBlock(ShaderStage::Vertex, "mvpsUBO", "mat4 mvps[512];");
    shaderCodeTest->SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", "mvps");
    shaderCodeTest->SetMain(ShaderStage::Vertex,
    "int objID = " + objIDString + ";\n"
    "colorOut = aColor;\n"
    "texCoordOut = aTexCoord;\n"
    "matID = objID;\n"
    "gl_Position = mvps[objID]*vec4(aPos, 1.0);");
    shaderCodeTest->SetStageToPipeline(ShaderStage::Fragment, true);
    shaderCodeTest->DefineMaterialParametersStruct(ShaderStage::Fragment, "Material");
    shaderCodeTest->AddMaterialVec4ToStruct("Material", ShaderStage::Fragment, "albedo");
    shaderCodeTest->AddMaterialFloatToStruct("Material", ShaderStage::Fragment, "intensity", 1.0f);

    Ref<Texture> defaultTexture;
    {
        int width = 256;
        int height = 256;
        int pixelsCount = width * height;
        std::vector<GLubyte> pixels;
        pixels.reserve(pixelsCount);
        for(int i = 0; i < pixelsCount; i++){
            pixels.push_back(0xFF);
            pixels.push_back(0xFF);
            pixels.push_back(0xFF);
        }
        defaultTexture = CreateRef<Texture>();
        defaultTexture->LoadFromMemory(pixels, gli::format::FORMAT_RGB8_SRGB_PACK8, width, height);
    }
    Ref<Texture> tex0;
    {
        tex0 = CreateRef<Texture>();
        if(!tex0->Load(std::filesystem::path("../resources/images/need-for-speed-carbon.jpg").generic_string(), true, true)){
            tex0 = defaultTexture;
        }
    }

    shaderCodeTest->AddMaterialMapArray(ShaderStage::Fragment, "texArray", defaultTexture);

    shaderCodeTest->UpdateMaterialParameterUniformBlock(ShaderStage::Fragment, "matUBO", "Material materials[512];");
    shaderCodeTest->SetBindingPurpose(ShaderStage::Fragment, "matUBO", "materials");
    shaderCodeTest->AddOutput(ShaderStage::Fragment, "FragColor", ShaderDataType::Float4);
    shaderCodeTest->SetMain(ShaderStage::Fragment,
    "vec4 albedo = materials[matID].albedo;\n"
    "float intensity = materials[matID].intensity;\n"
    "vec4 baseColor = intensity * albedo;\n"
    "FragColor = baseColor * texture(texArray, vec3(texCoordOut.x, texCoordOut.y, matID));");
    Ref<Material> materialTest = CreateRef<Material>(shaderStandard);
    Ref<Material> materialTestGreen = CreateRef<Material>(shaderStandard);
    Ref<Material> materialTestRed = CreateRef<Material>(shaderStandard);
    materialTest->SetParameterMap("diffuseMap", tex0);
    materialTest->SetFlag("lighting", false);
    materialTestGreen->SetParameterMap("diffuseMap", tex0);
    materialTestGreen->SetFlag("lighting", false);
    materialTestRed->SetParameterMap("diffuseMap", defaultTexture);
    materialTestRed->SetFlag("lighting", false);

    Scene mainScene;
    Entity quad1 = mainScene.CreateEntity();
    Entity tri1 = mainScene.CreateEntity();
    Entity quad2 = mainScene.CreateEntity();
    Entity graph = mainScene.CreateEntity();
    Entity mainCamera = mainScene.CreateEntity();
    // Rotation Euler angles +X = Look Down; +Y = Look Right
    // Left handed system is ok with rotations: Positive rotations are clockwise and z+ points into screen
    TransformComponent freeCameraTransform;
    freeCameraTransform.position = glm::vec3(0, 1, 3);
    //freeCameraTransform.rotation = glm::quat(glm::vec3(0, 180, 0));
    TransformComponent topDownCameraTransform;
    topDownCameraTransform.position = glm::vec3(0, 5, 0);
    topDownCameraTransform.eulerAngles(glm::vec3(90, 0, 0));

    quad1.AddComponent<MeshRendererComponent>(mesh, materialTest);
    quad1.GetComponent<TransformComponent>().position = glm::vec3(0, 0, 0);

    tri1.AddComponent<MeshRendererComponent>(mesh2, materialTestGreen);
    tri1.GetComponent<TransformComponent>().position = glm::vec3(0, 1, 0);

    quad2.AddComponent<MeshRendererComponent>(mesh, materialTestRed);
    quad2.GetComponent<TransformComponent>().position = glm::vec3(0, -1, 0);

    graph.AddComponent<MeshRendererComponent>(graphMesh, graphMaterial);
    graph.GetComponent<TransformComponent>().position = glm::vec3(0, 0, 0);

    // Descriptor of models
    // FlipUVs is true for default - This is correct if y-axis convention is equal OpenGL
    // If y-axis convention is opposite of OpenGL, set FlipUVs to false
    // This is because image loading orientation is opposite of OpenGL (origin in top)
    struct ModelDescriptor{
        std::string path;
        bool flipUVs = true;
        ModelDescriptor(const std::string &path):path(path){}
        ModelDescriptor(const std::string &path, bool flipUVs):path(path), flipUVs(flipUVs){}
    };
    std::vector<ModelDescriptor> modelsDescriptors;
    modelsDescriptors.emplace_back("../resources/modelos/sponza/Sponza.gltf");
    modelsDescriptors.emplace_back("../resources/modelos/porsche/scene.gltf");
    modelsDescriptors.emplace_back("../resources/modelos/backpack/backpack.obj", false);
    std::vector<Model> models(modelsDescriptors.size());
    std::vector<Entity> sceneObjects;
    auto loadBegin = std::chrono::high_resolution_clock::now();
    tbb::parallel_for(0, static_cast<int>(modelsDescriptors.size()), [&](int i){
        Model model = Model();
        if(!model.Load(modelsDescriptors[i].path, shaderStandard, true, modelsDescriptors[i].flipUVs))
            return;
        models[i] = model;
    });
    for(auto& model : models){
        for(auto &component : model.GetComponents()){
            Entity ent = mainScene.CreateEntity();
            ent.AddComponent<MeshRendererComponent>(component.first.mesh.object, component.first.material.object);
            ent.transform = component.second;
            sceneObjects.push_back(ent);
        }
    }
    auto loadEnd = std::chrono::high_resolution_clock::now();
    fmt::print("Time to load models {0} (ms)\n", std::chrono::duration_cast<std::chrono::milliseconds>(loadEnd-loadBegin).count());

    mainCamera.AddComponent<CameraComponent>().isMain = true;

    // Entity sunLight = mainScene.CreateEntity();
    // sunLight.AddComponent<LightComponent>().color = glm::vec3(1.0f, 1.0f, 1.0f);
    // sunLight.GetComponent<LightComponent>().type = LightType::Directional;
    // sunLight.transform.position = glm::vec3(0, 5, 0);
    // sunLight.transform.rotation = glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));

    // Entity testLight = mainScene.CreateEntity();
    // testLight.AddComponent<LightComponent>().color = glm::vec3(1.0f, 1.0f, 1.0f);
    // testLight.GetComponent<LightComponent>().range = 15.0f;
    // testLight.GetComponent<LightComponent>().type = LightType::Spot;
    // testLight.transform.position = glm::vec3(0, 5.0f, 0);
    // testLight.transform.rotation = glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));

    int lightCount = 15;
    std::vector<Entity> lights;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> disInt(0, 2);
    std::uniform_real_distribution<float> disFloat(0, 1.0f);
    for(int i = 0; i < lightCount; i++){
        Entity light = mainScene.CreateEntity();
        int lightChannel = disInt(e);
        glm::vec3 color;
        if(lightChannel == 0)
            color = glm::vec3(1.0f, disFloat(e)*0.1f + 0.1f, disFloat(e)*0.1f + 0.1f);
        else if(lightChannel == 1)
            color = glm::vec3(disFloat(e)*0.1f + 0.1f, 1.0f, disFloat(e)*0.1f + 0.1f);
        else if(lightChannel == 2)
            color = glm::vec3(disFloat(e)*0.1f + 0.1f, disFloat(e)*0.1f + 0.1f, 1.0f);
        else
            color = glm::vec3(1.0f, 1.0f, 1.0f);
        light.AddComponent<LightComponent>().color = color;
        light.GetComponent<LightComponent>().range = 5.0f;
        //light.transform.position = glm::vec3(3*glm::cos(glm::radians((360.0f/lightCount)*i)), 3, 3*glm::sin(glm::radians((360.0f/lightCount)*i)));
        float maxX = 8.0f;
        float minX = -8.0f;
        float maxZ = 6.0f;
        float minZ = -6.0f;
        float maxY = 6.0f;
        float minY = 1.0f;
        float randomX = disFloat(e)*(maxX-minX) + minX;
        float randomY = disFloat(e)*(maxY-minY) + minY;
        float randomZ = disFloat(e)*(maxZ-minZ) + minZ;
        light.transform.position = glm::vec3(randomX, randomY, randomZ);
        lights.push_back(light);
    }

    Renderer mainRenderer = Renderer();
    mainRenderer.SetMainWindow(std::addressof(window));
    mainRenderer.SetInterleaveAttribState(false);
    mainRenderer.SetDepthPrepassState(true);
    double initialRendererTime = SDL_GetTicks();
    mainRenderer.Start(mainScene.registry);
    double prepareTime = (SDL_GetTicks() - initialRendererTime)/1000;
    fmt::print("\nTime to prepare meshes for grouping: {0:.2f} (ms)\n", 1000*prepareTime);
    fmt::print("Computed draw groups: {0}\n\n", mainRenderer.GetDrawGroupsCount());

    double time = 0;
    double lastTime = 0;
    double deltaTime = 0;
    double maxDeltaTime = 0;
    double minDeltaTime = 0;
    unsigned long ticks = 0;

    mainCamera.transform = freeCameraTransform;
    // Camera parameters
    bool isFreeCamera = true;
    float factor = 15.0f;
    float interpolationFactor = 25.0f;
    float xRotation = 0.0f;
    float yRotation = 180.0f;
    freeCameraTransform.rotation = glm::quat(glm::vec3(glm::radians(xRotation), glm::radians(yRotation), 0.0f));
    /* Loop until the user closes the window */
    bool running = true;
    while (running)
    {
        time = SDL_GetTicks()/1000.0;
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
        // Input
        /* Poll for and process events */
        Input::Update();
        if(Input::GetQuitState()){
            running = false;
        }
        /////

        if (Input::GetKeyDown(SDLK_ESCAPE)){
            if(perfomanceCounter){
                fmt::print("Min Delta Time: {0:.2f} ms\n", 1000*minDeltaTime);
                fmt::print("Max Delta Time: {0:.2f} ms\n", 1000*maxDeltaTime);
                fmt::print("Ticks/Sec: {0:.2f}\n", ticks/time);
            }
            running = false;
        }

        if(isFreeCamera){
            glm::vec3 up = freeCameraTransform.Up();
            glm::vec3 right = freeCameraTransform.Right();
            glm::vec3 forward = freeCameraTransform.Forward();
            float cameraSpeed = static_cast<float>(2.0 * deltaTime);

            float horizontal = Input::GetAxis(Axis::Horizontal);
            float vertical= Input::GetAxis(Axis::Vertical);
            if (Input::GetKeyHeld(SDLK_SPACE)){
                freeCameraTransform.position += cameraSpeed * up;
            }
            freeCameraTransform.position += cameraSpeed * right * horizontal;
            freeCameraTransform.position += cameraSpeed * forward * vertical;

            float mouseX = factor*Input::GetMouseDeltaX()*deltaTime;
            float mouseY = factor*Input::GetMouseDeltaY()*deltaTime;
            xRotation -= mouseY;
            yRotation += mouseX;
            xRotation = glm::clamp(xRotation, -85.0f, 85.0f);
            // Construir os quatérnions separadamente
            // glm::quat qPitch = glm::angleAxis(glm::radians(xRotation), glm::vec3(1, 0, 0)); // Pitch (X)
            // glm::quat qYaw = glm::angleAxis(glm::radians(yRotation), glm::vec3(0, 1, 0));  // Yaw (Y)
            //freeCameraTransform.rotation = qYaw * qPitch;
            // Calcula o novo alvo de rotação com base nos ângulos
            //glm::quat targetRotation = glm::quat(glm::vec3(glm::radians(xRotation), glm::radians(yRotation), 0.0f));
            glm::quat targetRotation = glm::quat(glm::radians(glm::vec3(xRotation, yRotation, 0.0f)));

            // Taxa de interpolação (controla suavidade)
            float interpolationSpeed = interpolationFactor * deltaTime;

            // Interpola suavemente da rotação atual para a nova rotação alvo
            freeCameraTransform.rotation = glm::slerp(freeCameraTransform.rotation, targetRotation, interpolationSpeed);

            mainCamera.transform = freeCameraTransform;
        } else {
            mainCamera.transform = topDownCameraTransform;
        }
        if (Input::GetKeyDown(SDLK_T)){
            isFreeCamera = !isFreeCamera;
        }
        //mainLight.transform.position = glm::vec3(1.5f*glm::cos(time), 3, 1.5f*glm::sin(time));
        for(size_t i = 0; i < lights.size(); i++){
            if(lights[i].transform.position.y > 15.0f){
                lights[i].transform.position.y = 1.0f;
            } else{
                lights[i].transform.position.y += 7.5f*deltaTime;
            }

        }

        //quad1.transform.eulerAngles(glm::vec3(0, 30*time, 0));

        graph.GetComponent<MeshRendererComponent>().material->SetGlobalParameterFloat(timeString, time);
        graph.transform.eulerAngles(glm::vec3(0, -30*time, 0));
        // Rendering
        /* Render here */
        mainRenderer.Update(mainScene.registry, deltaTime);
        /* Swap front and back buffers */
        SDL_GL_SwapWindow(window.GetHandle());
    }
    SDL_GL_DestroyContext(SDL_GL_GetCurrentContext());
    SDL_DestroyWindow(window.GetHandle());
    SDL_Quit();
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
