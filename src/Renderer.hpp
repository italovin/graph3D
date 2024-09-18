#ifndef RENDERER_H
#define RENDERER_H
#include "Base.hpp"
#include "GLApiVersions.hpp"
#include "VertexArray.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include "System.hpp"
#include "Window.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <algorithm>
#include <chrono>
#include <memory>

class Renderer : public System{
private:
    Window* mainWindow = nullptr;
    int mvpDefaultBindingPoint = 0;
    int objectsIndexerDefaultLocation = 0;
    GLApiVersion version = GLApiVersion::V330;
    struct Buffer{
        GLuint name = 0;
        unsigned int bufferSize = 0;
        int stride = 0;
        int bindingPoint = 0;

        Buffer() = default;
        Buffer(GLuint name, unsigned int bufferSize, int stride, int bindingPoint):
        name(name), bufferSize(bufferSize), stride(stride), bindingPoint(bindingPoint){}
    };
    struct DrawElementsIndirectCommand{
        unsigned int  count;
        unsigned int  instanceCount;
        unsigned int  firstIndex;
        int           baseVertex;
        unsigned int  baseInstance;
    // Optional user-defined data goes here - if nothing, stride is 0
    };
    struct DrawCmdBuffer : Buffer{
        GLsizei commandsCount;
    };
    struct RenderGroup{
        VertexArray vao;
        Shader shader;
        std::vector<Buffer> attributesBuffers;
        Buffer indicesBuffer;
        Buffer objectIndexerBuffer;
        Buffer mvpsUniformBuffer;
        std::vector<std::reference_wrapper<TransformComponent>> transforms;
        std::vector<glm::mat4> mvps;
        Buffer materialUniformBuffer;
        std::vector<std::reference_wrapper<Material>> materials;
        GLenum mode; // Equivalent to Topology
        GLenum indicesType; // Equivalent to mesh indices data type
        MeshIndexType indicesTypeEnum;
        int indicesTypeSize;
        DrawCmdBuffer drawCmdBuffer;
        std::vector<DrawElementsIndirectCommand> commands;
    };
    std::vector<RenderGroup> renderGroups;
    GLenum GetDrawMode(MeshTopology topology);
    GLenum GetIndicesType(MeshIndexType type);
    void BufferSubDataMVPs(RenderGroup &renderGroup);
    void SetRenderGroupLayout(RenderGroup &renderGroup, MeshLayout &layout);
    void BindRenderGroupAttributesBuffers(RenderGroup &renderGroup);
    void DrawFunctionNonIndirect(RenderGroup &renderGroup);
    void DrawFunctionIndirect(RenderGroup &renderGroup);
    void SetDrawFunction();
    //Defaulft drawing is direct type
    void (Renderer::*DrawFunction)(RenderGroup&) = &Renderer::DrawFunctionNonIndirect;
    // Performs grouping operations for batching and instancing
    void PrepareRenderGroups(entt::registry &registry);
    // Executes the drawing at update call
    void Draw(const CameraComponent &mainCamera, const TransformComponent &mainCameraTransform);
public:
    void SetMVPBindingPoint(int bindingPoint);
    void SetObjectsIndexerLocation(int location);
    void SetAPIVersion(GLApiVersion version);
    void SetMainWindow(Window *mainWindow);
    void Start(entt::registry &registry) override;
    void Update(entt::registry &registry, float deltaTime) override;
    int GetDrawGroupsCount();
};
#endif
