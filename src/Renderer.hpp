#ifndef RENDERER_H
#define RENDERER_H
#include "Base.hpp"
#include "GLApiVersions.hpp"
#include "VertexArray.hpp"
#include "Camera.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include <map>
#include <algorithm>
#include <chrono>

class Renderer{
private:
    Camera* camera = nullptr;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
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
        GLenum mode; //Equivalent to Topology
        GLenum indicesType; //
        MeshIndexType indicesTypeEnum;
        int indicesTypeSize;
        DrawCmdBuffer drawCmdBuffer;
        std::vector<DrawElementsIndirectCommand> commands;
    };
    std::vector<RenderGroup> renderGroups;
    void (Renderer::*DrawFunction)(RenderGroup&) = nullptr;
    GLenum GetDrawMode(MeshTopology topology);
    GLenum GetIndicesType(MeshIndexType type);
    void BufferSubDataMVPs(RenderGroup &renderGroup);
    void SetRenderGroupLayout(RenderGroup &renderGroup, MeshLayout &layout);
    void BindRenderGroupAttributesBuffers(RenderGroup &renderGroup);
    void DrawFunctionNonIndirect(RenderGroup &renderGroup);
    void DrawFunctionIndirect(RenderGroup &renderGroup);
    void SetDrawFunction();
public:
    void SetMVPBindingPoint(int bindingPoint);
    void SetObjectsIndexerLocation(int location);
    void SetProjection(const glm::mat4 &projectionMatrix);
    void SetMainCamera(Camera *mainCamera);
    void SetAPIVersion(GLApiVersion version);
    void Prepare(entt::registry &registry);
    void Draw();
    int GetDrawGroupsCount();
};
#endif