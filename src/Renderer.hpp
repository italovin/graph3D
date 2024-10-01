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

struct Member {
    std::string name;
    size_t size;
    size_t alignment;
    size_t offset;
    size_t paddingBefore;
};
// Classe que armazena dinamicamente as structs com paddings adequados
class StructArray {
private:
// Função auxiliar para alinhar um valor ao próximo múltiplo de um alinhamento
size_t alignOffset(size_t offset, size_t alignment) {
    return (offset + alignment - 1) & ~(alignment - 1);
}
public:
    // Define os membros da struct, seus tamanhos e alinhamentos
    std::vector<Member> members;
    size_t structSize;  // Tamanho da struct com padding
    size_t numStructs;  // Número de structs no array
    std::vector<char> data;  // Buffer de dados para armazenar structs

    StructArray() = default;

    StructArray(const std::vector<Member>& members, size_t numStructs)
        : members(members), numStructs(numStructs), structSize(0) {

        calculateOffsetsAndPadding();
        allocateBuffer();
    }

    // Função para calcular os offsets e paddings de cada membro
    void calculateOffsetsAndPadding() {
        size_t currentOffset = 0;

        // Percorre os membros e ajusta os offsets e paddings
        for (auto& member : members) {
            // Alinha o offset atual para o alinhamento do membro
            size_t alignedOffset = alignOffset(currentOffset, member.alignment);

            // Calcula o padding antes deste membro
            member.paddingBefore = alignedOffset - currentOffset;

            // Define o offset deste membro
            member.offset = alignedOffset;

            // Avança o offset atual pelo tamanho do membro
            currentOffset = alignedOffset + member.size;
        }

        // Alinha o tamanho total da struct para múltiplos de 16 bytes (std140 rule)
        structSize = alignOffset(currentOffset, 16);
    }

    // Função para alocar o buffer de dados para o array de structs
    void allocateBuffer() {
        size_t totalSize = structSize * numStructs;
        data.resize(totalSize);  // Redimensiona o buffer de dados para conter todas as structs
        std::memset(data.data(), 0, totalSize);  // Inicializa o buffer com zeros
    }

    // Função para definir o valor de um membro específico dentro de uma struct no array
    template<typename T>
    void setMember(size_t structIndex, const std::string& memberName, const T& value) {
        // Encontrar o membro pelo nome
        for (const auto& member : members) {
            if (member.name == memberName) {
                size_t offset = structIndex * structSize + member.offset;
                std::memcpy(data.data() + offset, &value, sizeof(T));
                return;
            }
        }
        std::cerr << "Membro '" << memberName << "' não encontrado na struct!\n";
    }

    std::vector<char> &GetData(){
        return data;
    }
};
class Renderer : public System{
private:
    Window* mainWindow = nullptr;
    int mvpDefaultBindingPoint = 0;
    int objectsIndexerDefaultLocation = 0;
    std::unordered_map<std::string, int> uboBindingsPurposes;
    int uboBindingPointFree = 0;
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
    int AddUBOBindingPurpose(const std::string &purpose);
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
