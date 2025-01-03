#include "Renderer.hpp"
#include "RenderCapabilities.hpp"
#include "ShaderStandard.hpp"
#include "Constants.hpp"
#include <cstring>
#include <chrono>
// Implementation for StructArray
size_t StructArray::alignOffset(size_t offset, size_t alignment) {
    return (offset + alignment - 1) & ~(alignment - 1);
}
StructArray::StructArray(const std::vector<Member>& members, size_t numStructs)
        : members(members), numStructs(numStructs) {

        calculateOffsetsAndPadding();
        allocateBuffer();
}
void StructArray::calculateOffsetsAndPadding() {
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

void StructArray::allocateBuffer() {
    size_t totalSize = structSize * numStructs;
    data.resize(totalSize);  // Redimensiona o buffer de dados para conter todas as structs
    std::memset(data.data(), 0, totalSize);  // Inicializa o buffer com zeros
}

template<typename T>
void StructArray::setMember(size_t structIndex, const std::string& memberName, const T& value) {
    // Encontrar o membro pelo nome
    std::vector<Member>::iterator memberIt = std::find_if(members.begin(), members.end(), [&memberName](const Member &member){
        return member.name == memberName;
    });
    if(memberIt == members.end())
        return;

    Member member = *memberIt;
    size_t offset = structIndex * structSize + member.offset;
    std::memcpy(data.data() + offset, &value, sizeof(T));
    return;

    std::cerr << "Membro '" << memberName << "' não encontrado na struct!\n";
}

std::vector<char> &StructArray::GetData(){
    return data;
}
////


GLenum Renderer::GetDrawMode(MeshTopology topology){
    switch(topology){
        case MeshTopology::Lines: return GL_LINES;
        case MeshTopology::LineStrip: return GL_LINE_STRIP;
        case MeshTopology::Triangles: return GL_TRIANGLES;
        default: return GL_TRIANGLES;
    }
}

GLenum Renderer::GetIndicesType(MeshIndexType type){
    switch(type){
        case MeshIndexType::UnsignedInt: return GL_UNSIGNED_INT;
        case MeshIndexType::UnsignedShort: return GL_UNSIGNED_SHORT;
        default: return GL_UNSIGNED_INT;
    }
}

void Renderer::BufferSubDataMVPs(RenderGroup &renderGroup){
    glNamedBufferSubData(renderGroup.mvpsUniformBuffer.name, 0, sizeof(glm::mat4)*renderGroup.mvps.size(), renderGroup.mvps.data());
}

void Renderer::BufferSubDataModels(RenderGroup &renderGroup){
    glNamedBufferSubData(renderGroup.modelsUniformBuffer.name, 0, sizeof(glm::mat4)*renderGroup.models.size(), renderGroup.models.data());
}

void Renderer::BufferSubDataNormalMatrices(RenderGroup &renderGroup){
    glNamedBufferSubData(renderGroup.normalMatricesUniformBuffer.name, 0, sizeof(glm::mat4)*renderGroup.normalMatrices.size(), renderGroup.normalMatrices.data());
}

void Renderer::SetRenderGroupLayout(const RenderGroup &renderGroup, const MeshLayout &layout){
    int bindingPoint = 0;
    for(auto &&attribute : layout.attributes){
        GLenum type;
        GLboolean normalized;
        int locations = attribute.LocationsCount();
        switch(attribute.type){
            case MeshAttributeType::Float: type = GL_FLOAT; break;
            case MeshAttributeType::Int: type = GL_INT; break;
            case MeshAttributeType::UnsignedInt: type = GL_UNSIGNED_INT; break;
            case MeshAttributeType::Byte: type = GL_BYTE; break;
            case MeshAttributeType::UnsignedByte: type = GL_UNSIGNED_BYTE; break;
            case MeshAttributeType::Short: type = GL_SHORT; break;
            case MeshAttributeType::UnsignedShort: type = GL_UNSIGNED_SHORT; break;
            default: type = GL_FLOAT;
        }
        normalized = attribute.normalized ? GL_TRUE : GL_FALSE;
        int location = attribute.location;
        for(int i = location; i < location + locations; i++){
            if(attribute.interpretAsInt){
                glVertexArrayAttribIFormat(renderGroup.vao.GetHandle(), i, attribute.ScalarElementsCount(), type, 0);
            } else{
                glVertexArrayAttribFormat(renderGroup.vao.GetHandle(), i, attribute.ScalarElementsCount(), type, normalized, 0);
            }
            glEnableVertexArrayAttrib(renderGroup.vao.GetHandle(), i);
            glVertexArrayAttribBinding(renderGroup.vao.GetHandle(), i, bindingPoint);
        }
        bindingPoint++;
    }
}

void Renderer::BindRenderGroupAttributesBuffers(RenderGroup &renderGroup, const std::vector<GLintptr> &offsets, const std::vector<GLsizei> &strides)
{
    std::vector<GLuint> buffers(renderGroup.attributesCount, renderGroup.attributesBuffer.name);

    glVertexArrayVertexBuffers(renderGroup.vao.GetHandle(), 0, renderGroup.attributesCount, buffers.data(), offsets.data(), strides.data());
    glVertexArrayElementBuffer(renderGroup.vao.GetHandle(), renderGroup.indicesBuffer.name);
}

void Renderer::PrepareRenderGroups(entt::registry &registry){
    auto mapBegin = std::chrono::high_resolution_clock::now();
    auto renderableView = registry.view<MeshRendererComponent, TransformComponent>();

    std::vector<Renderable> componentsPairs;
    componentsPairs.reserve(renderableView.size_hint());
    for(auto entity : renderableView){
        auto &meshRenderer = renderableView.get<MeshRendererComponent>(entity);
        auto &transform = renderableView.get<TransformComponent>(entity);
        componentsPairs.emplace_back(meshRenderer, transform);
    }

    std::vector<ShaderGroup> shaderGroups;

    std::unordered_map<ResourceHandle, std::vector<Renderable>> shaderProgramMap;
    std::unordered_map<Shader, std::vector<Renderable>, ShaderHash> shaderModelMap;
    std::unordered_map<Shader, ShaderCode, ShaderHash> shaderCodeMap;
    std::unordered_map<ResourceHandle, GL::ShaderGLResource> shaderProgramCache;
    for(auto &&x: componentsPairs){
        auto &material = x.first.get().material;
        auto &mesh = x.first.get().mesh;
        auto shaderCast = dynamic_cast<ShaderStandard*>(x.first.get().material->GetShader().get());
        if(!shaderCast)
            continue; // Check if shader is a shader standard implementation
        ShaderStandard shader = *shaderCast;
        auto meshLayout = mesh->GetLayout();
        for(auto &&attribute : meshLayout.attributes){
            MeshAttributeAlias alias = attribute.alias;
            switch(alias){
                case MeshAttributeAlias::None: break;
                case MeshAttributeAlias::Position: shader.EnableAttribPosition(attribute.location); break;
                case MeshAttributeAlias::TexCoord0:shader.EnableAttribTexCoord_0(attribute.location); break;
                case MeshAttributeAlias::TexCoord1:shader.EnableAttribTexCoord_1(attribute.location); break;
                case MeshAttributeAlias::TexCoord2:shader.EnableAttribTexCoord_2(attribute.location); break;
                case MeshAttributeAlias::TexCoord3:shader.EnableAttribTexCoord_3(attribute.location); break;
                case MeshAttributeAlias::TexCoord4:shader.EnableAttribTexCoord_4(attribute.location); break;
                case MeshAttributeAlias::TexCoord5:shader.EnableAttribTexCoord_5(attribute.location); break;
                case MeshAttributeAlias::TexCoord6:shader.EnableAttribTexCoord_6(attribute.location); break;
                case MeshAttributeAlias::TexCoord7:shader.EnableAttribTexCoord_7(attribute.location); break;
                case MeshAttributeAlias::Normal: shader.EnableAttribNormal(attribute.location); break;
                case MeshAttributeAlias::Tangent:shader.EnableAttribTangent(attribute.location); break;
                case MeshAttributeAlias::Bitangent:shader.EnableAttribBiTangent(attribute.location); break;
                case MeshAttributeAlias::Color:shader.EnableAttribColor(attribute.location); break;
            }
        }
        auto materialMaps = material->GetMapParameters();
        for(auto &&materialMap : materialMaps){
            if(std::get<Ref<Texture>>(materialMap.second.data) == Ref<Texture>(nullptr))
                continue; // Parameter contain no map

            if(materialMap.first == "diffuseMap"){
                shader.ActivateDiffuseMap();
            } else if(materialMap.first == "specularMap"){
                shader.ActivateSpecularMap();
            } else if(materialMap.first == "normalMap"){
                shader.ActivateNormalMap();
            }
        }
        auto materialFlags = material->GetFlags();
        for(auto &&flag : materialFlags){
            if(flag.first == "lighting" && flag.second)
                shader.ActivateLighting();
        }

        if(shaderModelMap.count(shader) == 0){
            ShaderCode shaderCode = shader.ProcessCode();
            shaderCodeMap[shader] = shaderCode;
        }
        shaderModelMap[shader].push_back(std::move(x));
    }
    int64_t generateTimeTotal = 0;
    for(auto &&x : shaderModelMap){ // Group by while texture layers and mvps limits are not reached and then group by shader
        if(x.second.empty())
            continue;
        auto generateBegin = std::chrono::high_resolution_clock::now();
        // Generate shader program and resource
        GL::ShaderGLResource shaderGeneratedGlobal(shaderCodeMap[x.first].Generate());
        if(!shaderGeneratedGlobal.object)
            continue;
        auto generateEnd = std::chrono::high_resolution_clock::now();
        int64_t generateTime = std::chrono::duration_cast<std::chrono::microseconds>(generateEnd-generateBegin).count();
        generateTimeTotal += generateTime;

        const size_t maxTextureArrayLayers = RenderCapabilities::GetMaxTextureArrayLayers();  // Máximo de texturas
        const size_t maxUBOMatrices = glm::min(
            RenderCapabilities::GetMaxUBOSize() / sizeof(glm::mat4), Constants::maxObjectsToGroup
        );
        size_t mapTypeCount = x.second[0].first.get().material->GetActivatedMapParameters().size();
        std::vector<std::vector<Renderable>> shaderGeneratedGroups;
        shaderGeneratedGroups.reserve((x.second.size() + maxUBOMatrices - 1) / maxUBOMatrices); // Estimar o número de grupos
        std::vector<std::unordered_set<Texture*>> currentGroupTextures(mapTypeCount); // Texturas únicas por tipo de mapa
        std::vector<Renderable> currentGroup;                            // Objetos no grupo atual
        for (auto& renderable : x.second) {
            bool exceedsTextureLimit = false;

            // Verificar texturas únicas para cada tipo de mapa
            auto activatedMaps = renderable.first.get().material->GetActivatedMapParameters();
            for (size_t mapType = 0; mapType < mapTypeCount; ++mapType) {
                if (currentGroupTextures[mapType].find(std::get<Ref<Texture>>(activatedMaps[mapType].second.data).get()) 
                    == currentGroupTextures[mapType].end() &&
                    currentGroupTextures[mapType].size() >= maxTextureArrayLayers) {
                    exceedsTextureLimit = true;
                    break;
                }
            }

            // Verificar se o grupo atingiu limites
            if (currentGroup.size() >= maxUBOMatrices || exceedsTextureLimit) {
                // Finalizar o grupo atual
                shaderGeneratedGroups.push_back(std::move(currentGroup));
                for (auto& textureSet : currentGroupTextures) {
                    textureSet.clear(); // Limpar texturas únicas
                }
                currentGroup.clear(); // Limpar objetos
            }

            // Adicionar o renderable ao grupo atual
            currentGroup.push_back(std::move(renderable));

            // Atualizar texturas únicas por tipo de mapa
            for (size_t mapType = 0; mapType < mapTypeCount; ++mapType) {
                currentGroupTextures[mapType].insert(std::get<Ref<Texture>>(activatedMaps[mapType].second.data).get());
            }
        }

        // Adicionar o último grupo se não estiver vazio
        if (!currentGroup.empty()) {
            shaderGeneratedGroups.push_back(std::move(currentGroup));
        }
        for(auto &&group : shaderGeneratedGroups){
            GL::ShaderGLResource shaderGenerated(shaderGeneratedGlobal.object);
            // Setting shaders binding points
            for(auto &&bindingPurpose : shaderCodeMap[x.first].GetBindingsPurposes(ShaderStage::Vertex)){
                std::optional<int> binding = AddUBOBindingPurpose(bindingPurpose.second);
                if(!binding.has_value()){
                    // No binding point available
                    return;
                }
                shaderGenerated->SetBlockBinding(bindingPurpose.first, uboBindingsPurposes[bindingPurpose.second]);
            }
            for(auto &&bindingPurpose : shaderCodeMap[x.first].GetBindingsPurposes(ShaderStage::Fragment)){
                std::optional<int> binding = AddUBOBindingPurpose(bindingPurpose.second);
                if(!binding.has_value()){
                    // No binding point available
                    return;
                }
                shaderGenerated->SetBlockBinding(bindingPurpose.first, uboBindingsPurposes[bindingPurpose.second]);
            }
            auto &programGroup = shaderProgramMap[shaderGenerated.resourceHandle];
            programGroup.insert(programGroup.end(), std::make_move_iterator(group.begin()), std::make_move_iterator(group.end()));
            shaderProgramCache[shaderGenerated.resourceHandle] = shaderGenerated;
        }
    }
    // glm::ivec2 already have equal operator
    struct IVec2Hash {
        std::size_t operator()(const glm::ivec2& vec) const {
            std::size_t h1 = std::hash<int>{}(vec.x);
            std::size_t h2 = std::hash<int>{}(vec.y);
            return h1 ^ (h2 << 1); // Combina os hashes
        }
    };
    struct VectorIVec2Hash {
    std::size_t operator()(const std::vector<glm::ivec2>& vec) const {
        std::size_t seed = 0;
        for (const auto& v : vec) {
            seed ^= IVec2Hash{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
        }
    };
    // Comparação personalizada para std::vector<glm::ivec2>
    struct VectorIVec2Equal {
        bool operator()(const std::vector<glm::ivec2>& a, const std::vector<glm::ivec2>& b) const {
            return a == b;
        }
    };
    using MapsDimensionKey = std::vector<glm::ivec2>; // Used to compare texture maps dimensions
    std::unordered_map<MapsDimensionKey, std::unordered_map<ResourceHandle, std::vector<Renderer::Renderable>>, 
    VectorIVec2Hash, VectorIVec2Equal> sameTextureDimensionMap;

    for(auto &&group : shaderProgramMap){
        for(auto &&x: group.second){
            auto texParameters = x.first.get().material->GetActivatedMapParameters();
            std::vector<glm::ivec2> texDimensions;
            texDimensions.reserve(texParameters.size());
            for(auto &&map : texParameters){
                const auto& tex = std::get<Ref<Texture>>(map.second.data);
                texDimensions.push_back(glm::ivec2(tex->GetWidth(), tex->GetHeight()));
            }
            sameTextureDimensionMap[texDimensions][group.first].push_back(std::move(x));
        }
    }
    {
        size_t totalGroups = 0; // Total groups to be reserved in shaderGroups
        for (const auto& map : sameTextureDimensionMap) {
            totalGroups += map.second.size(); // Conta o número de grupos para cada dimensão de textura
        }
        shaderGroups.reserve(totalGroups);
    }
    
    for(auto &&map : sameTextureDimensionMap){
        for(auto &&group : map.second){
            ShaderGroup shaderGroup;
            shaderGroup.shader = shaderProgramCache[group.first].object;
            std::unordered_map<Mesh*, std::vector<Renderable>> groupMap;
            groupMap.reserve(group.second.size());
            int batchSize = 0; // Number of elements in batch group
            int instancesSize = 0; // Number of instances groups
            for(auto &&x : group.second){
                if(groupMap[x.first.get().mesh.object.get()].size() == 0){
                    batchSize++;
                } else if(groupMap[x.first.get().mesh.object.get()].size() == 1){
                    batchSize--; // Reverts when instances (duplied meshes) are found
                    instancesSize++; // Add instance group count when there is duplied mesh
                }
                groupMap[x.first.get().mesh.object.get()].push_back(std::move(x));
            }
            shaderGroup.batchGroup.reserve(batchSize);
            shaderGroup.instancesGroups.reserve(instancesSize);
            for(auto &&x : groupMap){
                if(x.second.size() >= 2){
                    shaderGroup.instancesGroups.push_back(std::move(x.second));
                } else {
                    shaderGroup.batchGroup.insert(
                        shaderGroup.batchGroup.end(),
                        std::make_move_iterator(x.second.begin()),
                        std::make_move_iterator(x.second.end())
                    );
                }
            }
            shaderGroups.push_back(std::move(shaderGroup));
        }
    }
    // In the end, shaderGroups are groups which objects uses the same shader program (resultant of the
    // same mesh layout, same maps layout and same flags activated layout) and the 
    // same texture dimensions for each map type
    auto mapEnd = std::chrono::high_resolution_clock::now();
    int64_t mapTimeTotal = std::chrono::duration_cast<std::chrono::microseconds>(mapEnd-mapBegin).count();
    // Print shader mapping time: time to batch the rendergroups with the shaders
    std::cout << "\nTime to shaders mapping: " << mapTimeTotal << " (μs)\n";
    std::cout << "Time to generate shaders only: " << generateTimeTotal << " (μs)\n";
    // Create needed render groups with VAO initialization
    renderGroups.resize(shaderGroups.size());
    for(size_t i = 0; i < renderGroups.size(); i++){
        renderGroups[i].shader = shaderGroups[i].shader;
        std::cout << "\n-- Building render group " << i + 1 << "\n";
        BuildRenderGroup(renderGroups[i], shaderGroups[i]);
    }
}

std::optional<int> Renderer::AddUBOBindingPurpose(const std::string &purpose){
    int bindingPoint = -1;

    if(uboBindingsPurposes.count(purpose) == 0){
        for(auto &&availableBinding : availableBindingPoints){
            if(availableBinding.second){
                bindingPoint = availableBinding.first;
                availableBindingPoints[bindingPoint] = false;
                break;
            }
        }
        if(bindingPoint == -1)
            return std::nullopt;

        uboBindingsPurposes.emplace(purpose, bindingPoint);
    } else {
        bindingPoint = uboBindingsPurposes[purpose];
    }
    return bindingPoint;
}

void Renderer::SetDrawFunction(){
    bool drawIndirectSupport = version >= GLApiVersion::V400;
    this->isIndirect = drawIndirectSupport;
    DrawFunction = drawIndirectSupport ?
    &Renderer::DrawFunctionIndirect : &Renderer::DrawFunctionNonIndirect;
}

void Renderer::BuildRenderGroup(RenderGroup &renderGroup, const ShaderGroup &shaderGroup)
{
    auto setupBegin = std::chrono::high_resolution_clock::now();
    
    MeshLayout layout;

    auto& batchGroup = shaderGroup.GetBatchGroup();
    auto& instancesGroups = shaderGroup.GetInstancesGroups();
    size_t objectsCount = batchGroup.size();

    if(!std::all_of(batchGroup.begin(), batchGroup.end(), [&](Renderable pair){
        return (pair.first.get().mesh->GetTopology() == batchGroup[0].first.get().mesh->GetTopology()
        && (pair.first.get().mesh->GetIndicesType() == batchGroup[0].first.get().mesh->GetIndicesType()));
    })){
        return;
    }
    for(auto&& instanceGroup : instancesGroups){
        // Adding objects from instancing grouping to object counter
        objectsCount += instanceGroup.size();
        if(!std::all_of(instanceGroup.begin(), instanceGroup.end(), [&](Renderable pair){
        return (pair.first.get().mesh->GetTopology() == instanceGroup[0].first.get().mesh->GetTopology())
        && (pair.first.get().mesh->GetIndicesType() == instanceGroup[0].first.get().mesh->GetIndicesType());
        })){
            return;
        }
    }

    MeshLayout meshGlobalLayout;
    MeshTopology meshGlobalTopology;
    MeshIndexType meshGlobalIndicesType;

    if(batchGroup.size() > 0 && !(instancesGroups.size() > 0 && instancesGroups[0].size() > 0)){
        meshGlobalLayout = batchGroup[0].first.get().mesh->GetLayout();
        meshGlobalTopology = batchGroup[0].first.get().mesh->GetTopology();
        meshGlobalIndicesType = batchGroup[0].first.get().mesh->GetIndicesType();
    } else if(instancesGroups.size() > 0 && instancesGroups[0].size() > 0 && !(batchGroup.size() > 0)){
        meshGlobalLayout = instancesGroups[0][0].first.get().mesh->GetLayout();
        meshGlobalTopology = instancesGroups[0][0].first.get().mesh->GetTopology();
        meshGlobalIndicesType = instancesGroups[0][0].first.get().mesh->GetIndicesType();
    } else if(instancesGroups.size() > 0 && instancesGroups[0].size() > 0){
        if(!(batchGroup[0].first.get().mesh->GetLayout() == instancesGroups[0][0].first.get().mesh->GetLayout()) ||
        !(batchGroup[0].first.get().mesh->GetTopology() == instancesGroups[0][0].first.get().mesh->GetTopology()) ||
        !(batchGroup[0].first.get().mesh->GetIndicesType() == instancesGroups[0][0].first.get().mesh->GetIndicesType())){
            return;
        }
        meshGlobalLayout = batchGroup[0].first.get().mesh->GetLayout();
        meshGlobalTopology = batchGroup[0].first.get().mesh->GetTopology();
        meshGlobalIndicesType = batchGroup[0].first.get().mesh->GetIndicesType();
    } else {
        // In the case that batch and instances groups doesn't exist
        return;
    }

    renderGroup.mode = GetDrawMode(meshGlobalTopology);
    renderGroup.indicesType = GetIndicesType(meshGlobalIndicesType);
    renderGroup.indicesTypeEnum = meshGlobalIndicesType;
    renderGroup.indicesTypeSize = Mesh::GetIndicesTypeSize(renderGroup.indicesTypeEnum);

    int attributesCount = meshGlobalLayout.attributes.size();

    //Combines meshes attributes in a single data vector
    std::vector<MeshAttributeData> attributesBatchedChunks(attributesCount);
    for(size_t i = 0; i < attributesBatchedChunks.size(); i++){
        attributesBatchedChunks[i].attribute = meshGlobalLayout.attributes[i];
        switch(attributesBatchedChunks[i].attribute.type){
            case MeshAttributeType::Float:
            attributesBatchedChunks[i].data = std::vector<float>(); break;
            case MeshAttributeType::Int:
            attributesBatchedChunks[i].data = std::vector<int>(); break;
            case MeshAttributeType::UnsignedInt:
            attributesBatchedChunks[i].data = std::vector<unsigned int>(); break;
            case MeshAttributeType::Byte:
            attributesBatchedChunks[i].data = std::vector<char>(); break;
            case MeshAttributeType::UnsignedByte:
            attributesBatchedChunks[i].data = std::vector<unsigned char>(); break;
            case MeshAttributeType::Short:
            attributesBatchedChunks[i].data = std::vector<short>(); break;
            case MeshAttributeType::UnsignedShort:
            attributesBatchedChunks[i].data = std::vector<unsigned short>(); break;
            case MeshAttributeType::None: break;
        }
    }

    MeshIndexData indicesBatchedChunk;
    indicesBatchedChunk.type = meshGlobalIndicesType;

    switch(indicesBatchedChunk.type){
        case MeshIndexType::UnsignedInt:
        indicesBatchedChunk.indices = std::vector<unsigned int>(); break;
        case MeshIndexType::UnsignedShort:
        indicesBatchedChunk.indices = std::vector<unsigned short>(); break;
        case MeshIndexType::None: break;
    }

    //Temp auxiliary variables
    int objectIndex = 0; // Indexer for every object
    int meshIndex = 0; // Index for each mesh
    int baseVertex = 0;
    unsigned int firstIndex = 0;
    unsigned int baseInstance = 0;

    renderGroup.mvps.reserve(objectsCount);
    renderGroup.models.reserve(objectsCount);
    renderGroup.normalMatrices.reserve(objectsCount);
    renderGroup.transforms.reserve(objectsCount);
    renderGroup.instancesGroups.reserve(instancesGroups.size());

    // Boolean to check if material parameters struct layout is calculated
    bool areParametersMembersSet = false;
    StructArray matParamStructArray;

    int textureParametersCount = 0;
    // Each texture array contains a map with texture pointer key and the index of layer
    // This will be used with a UBO (indexes); ubo[objID] -> layer index; tex(layer index)
    std::vector<std::unordered_map<Texture*, int>> texturesArraysImagesIndexMap;
    // Check if texture image is already storage in texture array structure
    std::unordered_map<Texture*, bool> texturesImagesStored;
    std::vector<std::vector<glm::ivec4>> texturesArraysIndices;
    // Map from array index to parameter map name 
    std::unordered_map<int, std::string> texturesArraysNamesMap;
    // Store max dimensions for build each texture array
    struct MaxTexDimensions{
        size_t maxWidth = 0;
        size_t maxHeight = 0;
    };
    // Calculating max texture dimensions for each texture array (each material map parameter)
    std::vector<MaxTexDimensions> maxTexDimensions;
    if(batchGroup.size() > 0){
        auto map = batchGroup[0].first.get().material->GetActivatedMapParameters();
        textureParametersCount = map.size();
        maxTexDimensions = std::vector<MaxTexDimensions>(textureParametersCount);
    } else if(instancesGroups.size() > 0){
        if(instancesGroups[0].size() > 0){
            auto map = instancesGroups[0][0].first.get().material->GetActivatedMapParameters();
            textureParametersCount = map.size();
            maxTexDimensions = std::vector<MaxTexDimensions>(textureParametersCount);
        } 
    }
    texturesArraysImagesIndexMap = std::vector<std::unordered_map<Texture*, int>>(textureParametersCount);
    texturesArraysIndices = std::vector<std::vector<glm::ivec4>>(textureParametersCount);
    for(auto &vec : texturesArraysIndices){
        vec = std::vector<glm::ivec4>(objectsCount);
    }

    {
        for(auto &&object : batchGroup){
            auto texParameters = object.first.get().material->GetActivatedMapParameters();
            int texParameterIndexer = 0;
            for(auto &&texParameter : texParameters){
                const auto& tex = std::get<Ref<Texture>>(texParameter.second.data);

                if(texturesArraysImagesIndexMap[texParameterIndexer].count(tex.get()) == 0){
                    int index = texturesArraysImagesIndexMap[texParameterIndexer].size();
                    texturesArraysImagesIndexMap[texParameterIndexer][tex.get()] = index;
                }
                texturesArraysNamesMap.try_emplace(texParameterIndexer, texParameter.first);
                texParameterIndexer++;
            }
        }
        for(auto &&instanceGroup : instancesGroups){
            for(auto &&object : instanceGroup){
                auto texParameters = object.first.get().material->GetActivatedMapParameters();
                int texParameterIndexer = 0;
                for(auto &&texParameter : texParameters){
                    const auto& tex = std::get<Ref<Texture>>(texParameter.second.data);

                    if(texturesArraysImagesIndexMap[texParameterIndexer].count(tex.get()) == 0){
                        int index = texturesArraysImagesIndexMap[texParameterIndexer].size();
                        texturesArraysImagesIndexMap[texParameterIndexer][tex.get()] = index;
                    }
                    texturesArraysNamesMap.try_emplace(texParameterIndexer, texParameter.first);
                    texParameterIndexer++;
                }
            }
        }
    }
    { // Setup textures arrays
        int objectIndexer = 0;
        bool texturesArraysInitialized = false; // Check if texture arrays are setup
        bool atLeastOneTexParameter = false; // At least one tex array map is setup
        for(auto &&object : batchGroup){
            auto texParameters = object.first.get().material->GetActivatedMapParameters();
            int texParameterIndexer = 0;
            for(auto &texParameter : texParameters){
                const auto& tex = std::get<Ref<Texture>>(texParameter.second.data);
                
                // Initialize textures arrays
                if(!texturesArraysInitialized){
                    Ref<GL::TextureGL> textureGL = CreateRef<GL::TextureGL>(GL_TEXTURE_2D_ARRAY);
                    //textureGL->SetupStorage3D(maxTexDimensions[texParameterIndexer].maxWidth, maxTexDimensions[texParameterIndexer].maxHeight, texturesArraysImagesIndexMap[texParameterIndexer].size());
                    textureGL->SetupStorage3D(tex->GetWidth(), tex->GetHeight(), texturesArraysImagesIndexMap[texParameterIndexer].size());
                    textureGL->SetupParameters();
                    renderGroup.texturesArrays.push_back(GL::TextureGLResource(textureGL));
                    renderGroup.shader->SetInt(texParameter.first, texParameterIndexer);
                    atLeastOneTexParameter = true;
                }
                texParameterIndexer++;
            }
            texturesArraysInitialized = atLeastOneTexParameter;
            objectIndexer++;
        }
        for(auto &&instanceGroup : instancesGroups){
            for(auto &&object : instanceGroup){
                auto texParameters = object.first.get().material->GetActivatedMapParameters();
                int texParameterIndexer = 0;
                for(auto &texParameter : texParameters){
                    const auto& tex = std::get<Ref<Texture>>(texParameter.second.data);

                    // Initialize textures arrays
                    if(!texturesArraysInitialized){
                        Ref<GL::TextureGL> textureGL = CreateRef<GL::TextureGL>(GL_TEXTURE_2D_ARRAY);
                        //textureGL->SetupStorage3D(maxTexDimensions[texParameterIndexer].maxWidth, maxTexDimensions[texParameterIndexer].maxHeight, texturesArraysImagesIndexMap[texParameterIndexer].size());
                        textureGL->SetupStorage3D(tex->GetWidth(), tex->GetHeight(), texturesArraysImagesIndexMap[texParameterIndexer].size());
                        textureGL->SetupParameters();
                        renderGroup.texturesArrays.push_back(GL::TextureGLResource(textureGL));
                        renderGroup.shader->SetInt(texParameter.first, texParameterIndexer);
                        atLeastOneTexParameter = true;
                    }
                    texParameterIndexer++;
                }
                texturesArraysInitialized = atLeastOneTexParameter;
                objectIndexer++;
            }
        }
    }
    // Uploading buffers and textures
    for(auto &&object : batchGroup){
        auto& mesh = object.first.get().mesh;

        size_t attributeIndex = 0;
        for(auto &&attributesData : mesh->GetAttributesDatas()){
            attributesBatchedChunks[attributeIndex].dataSize += attributesData.dataSize;
            switch(attributesData.attribute.type){
                case MeshAttributeType::Float:
                std::get<std::vector<float>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<float>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<float>>(attributesData.data).begin(),
                std::get<std::vector<float>>(attributesData.data).end()); break;
                case MeshAttributeType::Int:
                std::get<std::vector<int>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<int>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<int>>(attributesData.data).begin(),
                std::get<std::vector<int>>(attributesData.data).end()); break;
                case MeshAttributeType::UnsignedInt:
                std::get<std::vector<unsigned int>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<unsigned int>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<unsigned int>>(attributesData.data).begin(),
                std::get<std::vector<unsigned int>>(attributesData.data).end()); break;
                case MeshAttributeType::Byte:
                std::get<std::vector<char>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<char>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<char>>(attributesData.data).begin(),
                std::get<std::vector<char>>(attributesData.data).end()); break;
                case MeshAttributeType::UnsignedByte:
                std::get<std::vector<unsigned char>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<unsigned char>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<unsigned char>>(attributesData.data).begin(),
                std::get<std::vector<unsigned char>>(attributesData.data).end()); break;
                case MeshAttributeType::Short:
                std::get<std::vector<short>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<short>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<short>>(attributesData.data).begin(),
                std::get<std::vector<short>>(attributesData.data).end()); break;
                case MeshAttributeType::UnsignedShort:
                std::get<std::vector<unsigned short>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<unsigned short>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<unsigned short>>(attributesData.data).begin(),
                std::get<std::vector<unsigned short>>(attributesData.data).end()); break;
                case MeshAttributeType::None: break;
            }
            attributeIndex++;
        }

        {
            auto& indicesData = mesh->GetIndices();
            indicesBatchedChunk.indicesSize += indicesData.indicesSize;
            switch(indicesData.type){
                case MeshIndexType::UnsignedInt:
                std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).insert(
                std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).end(),
                std::get<std::vector<unsigned int>>(indicesData.indices).begin(),
                std::get<std::vector<unsigned int>>(indicesData.indices).end()); break;
                case MeshIndexType::UnsignedShort:
                std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).insert(
                std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).end(),
                std::get<std::vector<unsigned short>>(indicesData.indices).begin(),
                std::get<std::vector<unsigned short>>(indicesData.indices).end()); break;
                case MeshIndexType::None: break;
            }
        }

        unsigned int instanceCount = 1;
        unsigned int indicesCount = mesh->GetIndicesCount();
        if(isIndirect){
            // First Index - First in te index array (Offset in indices)
            // Base vertex - Base offset for the vertices
            DrawElementsIndirectCommand meshCmd(indicesCount, instanceCount, firstIndex, baseVertex, baseInstance);
            renderGroup.commands.push_back(std::move(meshCmd));
        } else {
            renderGroup.batchGroup.count.push_back(indicesCount);
            renderGroup.batchGroup.indices.push_back((GLvoid*)(intptr_t)(firstIndex*renderGroup.indicesTypeSize));
            renderGroup.batchGroup.drawcount += 1;
            renderGroup.batchGroup.baseVertex.push_back(baseVertex);
        }

        //Base vertex is offset of vertices, not of indices
        baseVertex += mesh->GetVerticesCount();
        firstIndex += indicesCount;
        baseInstance += instanceCount;

        auto& objectTransform = object.second;
        renderGroup.transforms.emplace_back(objectTransform);
        renderGroup.mvps.emplace_back(glm::mat4(1.0f));
        renderGroup.models.emplace_back(glm::mat4(1.0f));
        renderGroup.normalMatrices.emplace_back(glm::mat4(1.0f));

        auto objectMaterial = object.first.get().material;
        renderGroup.materials.emplace_back(*objectMaterial);
        // Register material callbacks
        objectMaterial->SetOnGlobalFloatChangeCallback([&renderGroup](const std::string &name, float value){
            renderGroup.shader->SetFloat(name, value);
        });
        objectMaterial->SetOnGlobalBooleanChangeCallback([&renderGroup](const std::string &name, bool value){
            renderGroup.shader->SetBool(name, value);
        });
        objectMaterial->SetOnGlobalVector4ChangeCallback([&renderGroup](const std::string &name, glm::vec4 value){
            renderGroup.shader->SetVec4(name, value);
        });

        auto matParameters = objectMaterial->GetParameters();
        // Setting material parameters layout once
        if(!areParametersMembersSet){
            std::vector<Member> members;
            for(auto &&parameter : matParameters){
                if(parameter.second.type == MaterialParameterType::Map)
                    continue;

                size_t mSize, mAlignment = 0;
                switch(parameter.second.type){
                    case MaterialParameterType::Boolean: mSize = 4; mAlignment = 4; break;
                    case MaterialParameterType::Float: mSize = 4; mAlignment = 4; break;
                    case MaterialParameterType::Vector4: mSize = 16; mAlignment = 16; break;
                    default: break;
                }
                members.emplace_back(parameter.first, mSize, mAlignment, 0, 0);
            }
            matParamStructArray = StructArray(members, objectsCount);
            areParametersMembersSet = true;
        }
        // Setting material struct array data
        if(areParametersMembersSet){
            for(auto &&parameter : matParameters){
                if(parameter.second.type == MaterialParameterType::Map)
                    continue;

                switch(parameter.second.type){
                    case MaterialParameterType::Float: matParamStructArray.setMember(objectIndex, parameter.first, std::get<float>(parameter.second.data)); break;
                    case MaterialParameterType::Boolean: matParamStructArray.setMember(objectIndex, parameter.first, std::get<bool>(parameter.second.data)); break;
                    case MaterialParameterType::Vector4: matParamStructArray.setMember(objectIndex, parameter.first, std::get<glm::vec4>(parameter.second.data));
                    default: break;
                }
            }

        }
        auto matTexParameters = objectMaterial->GetActivatedMapParameters();
        int texParameterIndexer = 0;
        for(auto &parameter : matTexParameters){
            const auto& texture = std::get<Ref<Texture>>(parameter.second.data);

            if(texturesImagesStored.count(texture.get()) == 0)
                texturesImagesStored[texture.get()] = false;
            int textureLayerIndex = texturesArraysImagesIndexMap[texParameterIndexer][texture.get()];
            if(!texturesImagesStored[texture.get()]){
                if(texture->GetPixels().dataType == TexturePixelDataType::UnsignedByte){
                    renderGroup.texturesArrays[texParameterIndexer]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), textureLayerIndex, texture->GetFormatGLenum(), std::get<std::vector<GLubyte>>(texture->GetPixels().data));
                } else if(texture->GetPixels().dataType == TexturePixelDataType::Float){
                    renderGroup.texturesArrays[texParameterIndexer]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), textureLayerIndex, texture->GetFormatGLenum(), std::get<std::vector<GLfloat>>(texture->GetPixels().data));
                }
                texturesImagesStored[texture.get()] = true;
            }
            texturesArraysIndices[texParameterIndexer][objectIndex] = glm::ivec4(textureLayerIndex,0,0,0);
            texParameterIndexer++;
        }

        objectIndex++;
        meshIndex++;
    }

    for(auto &&instanceGroup : instancesGroups){
        //All meshes in instance groups are the same
        auto& mesh = instanceGroup[0].first.get().mesh;
        unsigned int instanceCount = instanceGroup.size();

        int attributeIndex = 0;
        for(auto &&attributesData : mesh->GetAttributesDatas()){
            attributesBatchedChunks[attributeIndex].dataSize += attributesData.dataSize;
            switch(attributesData.attribute.type){
                case MeshAttributeType::Float:
                std::get<std::vector<float>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<float>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<float>>(attributesData.data).begin(),
                std::get<std::vector<float>>(attributesData.data).end()); break;
                case MeshAttributeType::Int:
                std::get<std::vector<int>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<int>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<int>>(attributesData.data).begin(),
                std::get<std::vector<int>>(attributesData.data).end()); break;
                case MeshAttributeType::UnsignedInt:
                std::get<std::vector<unsigned int>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<unsigned int>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<unsigned int>>(attributesData.data).begin(),
                std::get<std::vector<unsigned int>>(attributesData.data).end()); break;
                case MeshAttributeType::Byte:
                std::get<std::vector<char>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<char>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<char>>(attributesData.data).begin(),
                std::get<std::vector<char>>(attributesData.data).end()); break;
                case MeshAttributeType::UnsignedByte:
                std::get<std::vector<unsigned char>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<unsigned char>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<unsigned char>>(attributesData.data).begin(),
                std::get<std::vector<unsigned char>>(attributesData.data).end()); break;
                case MeshAttributeType::Short:
                std::get<std::vector<short>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<short>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<short>>(attributesData.data).begin(),
                std::get<std::vector<short>>(attributesData.data).end()); break;
                case MeshAttributeType::UnsignedShort:
                std::get<std::vector<unsigned short>>(attributesBatchedChunks[attributeIndex].data).insert(
                std::get<std::vector<unsigned short>>(attributesBatchedChunks[attributeIndex].data).end(),
                std::get<std::vector<unsigned short>>(attributesData.data).begin(),
                std::get<std::vector<unsigned short>>(attributesData.data).end()); break;
                case MeshAttributeType::None: break;
            }
            attributeIndex++;
        }

        {
            auto& indicesData = mesh->GetIndices();
            indicesBatchedChunk.indicesSize += indicesData.indicesSize;
            switch(indicesData.type){
                case MeshIndexType::UnsignedInt:
                std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).insert(
                std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).end(),
                std::get<std::vector<unsigned int>>(indicesData.indices).begin(),
                std::get<std::vector<unsigned int>>(indicesData.indices).end()); break;
                case MeshIndexType::UnsignedShort:
                std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).insert(
                std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).end(),
                std::get<std::vector<unsigned short>>(indicesData.indices).begin(),
                std::get<std::vector<unsigned short>>(indicesData.indices).end()); break;
                case MeshIndexType::None: break;
            }
        }

        unsigned int indicesCount = mesh->GetIndicesCount();
        if(isIndirect){
            // First Index - First in te index array (Offset in indices)
            // Base vertex - Base offset for the vertices
            DrawElementsIndirectCommand meshCmd(indicesCount, instanceCount, firstIndex, baseVertex, baseInstance);
            renderGroup.commands.push_back(std::move(meshCmd));
        } else {
            InstanceGroup instanceGroupToPush;
            instanceGroupToPush.count = indicesCount;
            instanceGroupToPush.firstIndex = firstIndex;
            instanceGroupToPush.instanceCount = instanceCount;
            instanceGroupToPush.baseVertex = baseVertex;
            instanceGroupToPush.baseInstance = baseInstance;
            renderGroup.instancesGroups.push_back(std::move(instanceGroupToPush));
        }

        //Base vertex is offset of vertices, not of indices
        baseVertex += mesh->GetVerticesCount();
        firstIndex += mesh->GetIndicesCount();
        baseInstance += instanceCount;

        for(auto &&object : instanceGroup){
            //Transform
            auto& objectTransform = object.second;
            renderGroup.transforms.push_back(objectTransform);
            renderGroup.mvps.emplace_back(glm::mat4(1.0f));
            renderGroup.models.emplace_back(glm::mat4(1.0f));
            renderGroup.normalMatrices.emplace_back(glm::mat4(1.0f));
            //Material
            auto objectMaterial = object.first.get().material;
            renderGroup.materials.push_back(*objectMaterial);
            // Register material callbacks
            objectMaterial->SetOnGlobalFloatChangeCallback([&renderGroup](const std::string &name, float value){
                renderGroup.shader->SetFloat(name, value);
            });
            objectMaterial->SetOnGlobalBooleanChangeCallback([&renderGroup](const std::string &name, bool value){
                renderGroup.shader->SetBool(name, value);
            });
            objectMaterial->SetOnGlobalVector4ChangeCallback([&renderGroup](const std::string &name, glm::vec4 value){
                renderGroup.shader->SetVec4(name, value);
            });

            auto matParameters = objectMaterial->GetParameters();
            // Setting material parameters layout once
            if(!areParametersMembersSet){
                std::vector<Member> members;
                for(auto &&parameter : matParameters){
                    if(parameter.second.type == MaterialParameterType::Map)
                        continue;

                    size_t mSize, mAlignment = 0;
                    switch(parameter.second.type){
                        case MaterialParameterType::Boolean: mSize = 4; mAlignment = 4; break;
                        case MaterialParameterType::Float: mSize = 4; mAlignment = 4; break;
                        case MaterialParameterType::Vector4: mSize = 16; mAlignment = 16; break;
                        case MaterialParameterType::Map: break;
                    }
                    members.emplace_back(parameter.first, mSize, mAlignment, 0, 0);
                }
                matParamStructArray = StructArray(members, objectsCount);
                areParametersMembersSet = true;
            }
            // Setting material struct array data
            if(areParametersMembersSet){
                for(auto &&parameter : matParameters){
                    if(parameter.second.type == MaterialParameterType::Map)
                        continue;

                    switch(parameter.second.type){
                        case MaterialParameterType::Float: matParamStructArray.setMember(objectIndex, parameter.first, std::get<float>(parameter.second.data)); break;
                        case MaterialParameterType::Boolean: matParamStructArray.setMember(objectIndex, parameter.first, std::get<bool>(parameter.second.data)); break;
                        case MaterialParameterType::Vector4: matParamStructArray.setMember(objectIndex, parameter.first, std::get<glm::vec4>(parameter.second.data));
                        default: break;
                    }
                }
            }
            auto matTexParameters = objectMaterial->GetActivatedMapParameters();
            int texParameterIndexer = 0;
            for(auto &parameter : matTexParameters){
                const auto& texture = std::get<Ref<Texture>>(parameter.second.data);

                if(texturesImagesStored.count(texture.get()) == 0)
                    texturesImagesStored[texture.get()] = false;
                int textureLayerIndex = texturesArraysImagesIndexMap[texParameterIndexer][texture.get()];
                if(!texturesImagesStored[texture.get()]){
                    if(texture->GetPixels().dataType == TexturePixelDataType::UnsignedByte){
                        renderGroup.texturesArrays[texParameterIndexer]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), textureLayerIndex, texture->GetFormatGLenum(), std::get<std::vector<GLubyte>>(texture->GetPixels().data));
                    } else if(texture->GetPixels().dataType == TexturePixelDataType::Float){
                        renderGroup.texturesArrays[texParameterIndexer]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), textureLayerIndex, texture->GetFormatGLenum(), std::get<std::vector<GLfloat>>(texture->GetPixels().data));
                    }
                    texturesImagesStored[texture.get()] = true;
                }
                texturesArraysIndices[texParameterIndexer][objectIndex] = glm::ivec4(textureLayerIndex,0,0,0);
                texParameterIndexer++;
            }

            objectIndex++;
        }
        meshIndex++;
    }
    // After all data is pushed, generate mipmaps for each texture array
    for(auto &texArray : renderGroup.texturesArrays){
        texArray->GenerateMipmaps();
    }
    renderGroup.objectsCount = objectsCount;
    renderGroup.materialsStructArray = matParamStructArray;

    renderGroup.attributesCount = attributesCount;
    int attributesBufferTotalSize = 0;
    std::vector<GLintptr> attributesOffsets(attributesCount);
    std::vector<GLsizei> attributesStrides(attributesCount);
    for(size_t i = 0; i < attributesBatchedChunks.size(); i++){
        attributesOffsets[i] = attributesBufferTotalSize;
        attributesStrides[i] = attributesBatchedChunks[i].attribute.AttributeDataSize();
        attributesBufferTotalSize += attributesBatchedChunks[i].dataSize;
    }
    auto setupEnd = std::chrono::high_resolution_clock::now();
    std::cout << "Time to setup batch and instance groups: " << std::chrono::duration_cast<std::chrono::microseconds>(setupEnd-setupBegin).count() << " (μs)\n";
    auto bufferBegin = std::chrono::high_resolution_clock::now();
    {
        GLuint attributesBuffersName = 0;
        glCreateBuffers(1, std::addressof(attributesBuffersName));
        renderGroup.attributesBuffer.name = attributesBuffersName;
        renderGroup.attributesBuffer.bufferSize = attributesBufferTotalSize;
        renderGroup.attributesBuffer.stride = attributesBufferTotalSize;
        renderGroup.attributesBuffer.bindingPoint = 0;
        glNamedBufferStorage(renderGroup.attributesBuffer.name, renderGroup.attributesBuffer.bufferSize,
        nullptr, GL_DYNAMIC_STORAGE_BIT);
        for(size_t i = 0; i < attributesBatchedChunks.size(); i++){
            switch(attributesBatchedChunks[i].attribute.type){
                case MeshAttributeType::Float:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<float>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::Int:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<int>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::UnsignedInt:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<unsigned int>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::Byte:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<char>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::UnsignedByte:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<unsigned char>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::Short:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<short>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::UnsignedShort:
                glNamedBufferSubData(renderGroup.attributesBuffer.name, attributesOffsets[i], attributesBatchedChunks[i].dataSize, std::get<std::vector<unsigned short>>(attributesBatchedChunks[i].data).data()); break;
                case MeshAttributeType::None: break;
            }
        }
    }
    {
        GLuint indicesBufferName = 0;
        glCreateBuffers(1, std::addressof(indicesBufferName));
        renderGroup.indicesBuffer.name = indicesBufferName;
        renderGroup.indicesBuffer.bufferSize = indicesBatchedChunk.indicesSize;
        switch(indicesBatchedChunk.type){
            case MeshIndexType::UnsignedInt:
            glNamedBufferStorage(renderGroup.indicesBuffer.name, renderGroup.indicesBuffer.bufferSize, std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).data(), GL_DYNAMIC_STORAGE_BIT); break;
            case MeshIndexType::UnsignedShort:
            glNamedBufferStorage(renderGroup.indicesBuffer.name, renderGroup.indicesBuffer.bufferSize, std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).data(), GL_DYNAMIC_STORAGE_BIT); break;
            case MeshIndexType::None: break;
        }
    }
    // MVP UBO
    {
        GLuint mpvsUniformBufferName = 0;
        glCreateBuffers(1, std::addressof(mpvsUniformBufferName));
        renderGroup.mvpsUniformBuffer.name = mpvsUniformBufferName;
        renderGroup.mvpsUniformBuffer.bufferSize = sizeof(glm::mat4)*renderGroup.mvps.size();
        renderGroup.mvpsUniformBuffer.stride = sizeof(glm::mat4);
        renderGroup.mvpsUniformBuffer.bindingPoint = uboBindingsPurposes["mvps"];
        glNamedBufferStorage(renderGroup.mvpsUniformBuffer.name, renderGroup.mvpsUniformBuffer.bufferSize, renderGroup.mvps.data(), GL_DYNAMIC_STORAGE_BIT);
    }
    // Models UBO
    {
        GLuint modelsUniformBufferName = 0;
        glCreateBuffers(1, std::addressof(modelsUniformBufferName));
        renderGroup.modelsUniformBuffer.name = modelsUniformBufferName;
        renderGroup.modelsUniformBuffer.bufferSize = sizeof(glm::mat4)*renderGroup.models.size();
        renderGroup.modelsUniformBuffer.stride = sizeof(glm::mat4);
        renderGroup.modelsUniformBuffer.bindingPoint = uboBindingsPurposes["models"];
        glNamedBufferStorage(renderGroup.modelsUniformBuffer.name, renderGroup.modelsUniformBuffer.bufferSize, renderGroup.models.data(), GL_DYNAMIC_STORAGE_BIT);
    }
    // Normal Matrices UBO
    {
        GLuint normalMatricesUniformBufferName = 0;
        glCreateBuffers(1, std::addressof(normalMatricesUniformBufferName));
        renderGroup.normalMatricesUniformBuffer.name = normalMatricesUniformBufferName;
        renderGroup.normalMatricesUniformBuffer.bufferSize = sizeof(glm::mat4)*renderGroup.normalMatrices.size();
        renderGroup.normalMatricesUniformBuffer.stride = sizeof(glm::mat4);
        renderGroup.normalMatricesUniformBuffer.bindingPoint = uboBindingsPurposes["normalMatrices"];
        glNamedBufferStorage(renderGroup.normalMatricesUniformBuffer.name, renderGroup.normalMatricesUniformBuffer.bufferSize, renderGroup.normalMatrices.data(), GL_DYNAMIC_STORAGE_BIT);
    }
    // Material UBO
    {
        GLuint materialUniformBufferName = 0;
        glCreateBuffers(1, std::addressof(materialUniformBufferName));
        renderGroup.materialUniformBuffer.name = materialUniformBufferName;
        renderGroup.materialUniformBuffer.bufferSize = renderGroup.materialsStructArray.structSize *renderGroup.materialsStructArray.numStructs;
        renderGroup.materialUniformBuffer.stride = renderGroup.materialsStructArray.structSize;
        renderGroup.materialUniformBuffer.bindingPoint = uboBindingsPurposes["materials"];
        if(renderGroup.materialUniformBuffer.bufferSize > 0)
            glNamedBufferStorage(renderGroup.materialUniformBuffer.name, renderGroup.materialUniformBuffer.bufferSize,
            renderGroup.materialsStructArray.GetData().data(), GL_DYNAMIC_STORAGE_BIT);
    }
    // Tex arrays indices UBO
    {
        std::vector<GLuint> texLayersIndexBuffersNames(texturesArraysIndices.size());
        glCreateBuffers(texturesArraysIndices.size(), texLayersIndexBuffersNames.data());
        for(int i = 0; i < textureParametersCount; i++){
            std::string bindingName;
            if(texturesArraysNamesMap[i] == "diffuseMap")
                bindingName = "diffuseMapIndices";
            else if(texturesArraysNamesMap[i] == "normalMap")
                bindingName = "normalMapIndices";
            else if(texturesArraysNamesMap[i] == "specularMap")
                bindingName = "specularMapIndices";
            else
                continue;
            renderGroup.texLayersIndexBuffers.emplace_back(texLayersIndexBuffersNames[i], sizeof(glm::ivec4)*objectsCount, 
            sizeof(int), uboBindingsPurposes[bindingName]);
            glNamedBufferStorage(renderGroup.texLayersIndexBuffers[i].name, renderGroup.texLayersIndexBuffers[i].bufferSize,
            texturesArraysIndices[i].data(), GL_DYNAMIC_STORAGE_BIT);
        }
    }

    SetRenderGroupLayout(renderGroup, meshGlobalLayout);
    BindRenderGroupAttributesBuffers(renderGroup, attributesOffsets, attributesStrides);

    if(isIndirect){
        GLuint drawCmdBufferName = 0;
        glCreateBuffers(1, std::addressof(drawCmdBufferName));
        renderGroup.drawCmdBuffer.name = drawCmdBufferName;
        renderGroup.drawCmdBuffer.bufferSize = sizeof(DrawElementsIndirectCommand) * renderGroup.commands.size();
        glNamedBufferStorage(renderGroup.drawCmdBuffer.name, renderGroup.drawCmdBuffer.bufferSize, renderGroup.commands.data(), GL_DYNAMIC_STORAGE_BIT);
    }


    auto bufferEnd = std::chrono::high_resolution_clock::now();
    std::cout << "Time to buffer data: " << std::chrono::duration_cast<std::chrono::microseconds>(bufferEnd-bufferBegin).count() << " (μs)\n";

    renderGroup.drawCmdBuffer.commandsCount = renderGroup.commands.size();
}

void Renderer::DrawFunctionIndirect(RenderGroup &renderGroup){
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderGroup.drawCmdBuffer.name);
    glMultiDrawElementsIndirect(
        renderGroup.mode,
        renderGroup.indicesType,
        static_cast<GLvoid *>(0),
        renderGroup.drawCmdBuffer.commandsCount,
        0);
}

void Renderer::DrawFunctionNonIndirect(RenderGroup &renderGroup){
    // Draw batch
    glMultiDrawElementsBaseVertex(
        renderGroup.mode,
        renderGroup.batchGroup.count.data(),
        renderGroup.indicesType,
        reinterpret_cast<GLvoid**>(renderGroup.batchGroup.indices.data()),
        renderGroup.batchGroup.drawcount,
        renderGroup.batchGroup.baseVertex.data()
    );
    // Draw instances
    for(auto && instanceGroup : renderGroup.instancesGroups){
        glDrawElementsInstancedBaseVertexBaseInstance(
        renderGroup.mode,
        instanceGroup.count,
        renderGroup.indicesType,
        reinterpret_cast<GLvoid *>(instanceGroup.firstIndex*renderGroup.indicesTypeSize),
        instanceGroup.instanceCount,
        instanceGroup.baseVertex,
        instanceGroup.baseInstance
        );
    }
}

void Renderer::SetMainWindow(Window *mainWindow){
    this->mainWindow = mainWindow;
}

void Renderer::Start(entt::registry &registry){
    PrepareRenderGroups(registry);
}

void Renderer::Update(entt::registry &registry, float deltaTime){
    auto cameraView = registry.view<CameraComponent, TransformComponent>();

    CameraComponent mainCamera;
    TransformComponent mainCameraTransform;
    for(auto entity : cameraView){
        auto &camera = cameraView.get<CameraComponent>(entity);
        auto &cameraTransform = cameraView.get<TransformComponent>(entity);
        if(camera.autoAspect)
            camera.aspectRatio = (float)mainWindow->GetWidth()/(float)mainWindow->GetHeight();
        if(camera.isMain){
            mainCamera = camera;
            mainCameraTransform = cameraTransform;
            break;
        }
    }
    LightComponent mainLight;
    TransformComponent mainLightTransform;

    auto lightView = registry.view<LightComponent, TransformComponent>();
    for(auto entity : lightView){
        auto &light = lightView.get<LightComponent>(entity);
        auto &lightTransform = lightView.get<TransformComponent>(entity);
        if(light.isMain){
            mainLight = light;
            mainLightTransform = lightTransform;
            break;
        }
    }
    Draw(mainCamera, mainCameraTransform, mainLight, mainLightTransform);
}

void Renderer::Draw(const CameraComponent &mainCamera, const TransformComponent &mainCameraTransform, const LightComponent &mainLight, const TransformComponent &lightTransform){
    glm::mat4 mainCameraProjection = mainCamera.isPerspective ?
    glm::perspectiveLH(glm::radians(mainCamera.fieldOfView), mainCamera.aspectRatio,
    mainCamera.nearPlane, mainCamera.farPlane) :
    glm::orthoLH(-mainCamera.aspectRatio * mainCamera.orthographicSize, mainCamera.aspectRatio * mainCamera.orthographicSize, -mainCamera.orthographicSize, mainCamera.orthographicSize, mainCamera.nearPlane, mainCamera.farPlane);

    glm::mat4 mainCameraView;
    {
        glm::mat4 rotate = glm::mat4_cast(glm::conjugate(mainCameraTransform.rotation));
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, -mainCameraTransform.position);
        mainCameraView = rotate * translate;
    }
    for(auto &&renderGroup : renderGroups){
        for(size_t i = 0; i < renderGroup.texturesArrays.size(); i++){
            renderGroup.texturesArrays[i]->Bind(i);
        }
        // Remember last shader program handle
        if(renderGroup.shader->GetHandle() != lastShaderProgram){
            renderGroup.shader->Use();
            lastShaderProgram = renderGroup.shader->GetHandle();
        }
        renderGroup.shader->SetVec3("lightPos", lightTransform.position);
        renderGroup.shader->SetVec3("viewPos", mainCameraTransform.position);
        renderGroup.shader->SetVec3("lightColor", mainLight.color);
        renderGroup.vao.Bind();

        for(int i = 0; i < renderGroup.objectsCount; i++){
            auto& transform = renderGroup.transforms[i];
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 scl = glm::scale(model, transform.get().scale);
            glm::mat4 rot = glm::mat4_cast(transform.get().rotation);
            glm::mat4 trn = glm::translate(model, transform.get().position);
            model = trn*rot*scl;
            renderGroup.mvps[i] = mainCameraProjection * mainCameraView * model;
            renderGroup.models[i] = model;
            renderGroup.normalMatrices[i] = glm::transpose(glm::inverse(model));
        }
        BufferSubDataMVPs(renderGroup); // Update MVPs of objects
        BufferSubDataModels(renderGroup); // Update models of objects
        BufferSubDataNormalMatrices(renderGroup); // Update normal matrices of objects
        // UBO bindings
        glBindBufferBase(GL_UNIFORM_BUFFER, renderGroup.mvpsUniformBuffer.bindingPoint, renderGroup.mvpsUniformBuffer.name);
        glBindBufferBase(GL_UNIFORM_BUFFER, renderGroup.modelsUniformBuffer.bindingPoint, renderGroup.modelsUniformBuffer.name);
        glBindBufferBase(GL_UNIFORM_BUFFER, renderGroup.normalMatricesUniformBuffer.bindingPoint, renderGroup.normalMatricesUniformBuffer.name);
        if(renderGroup.materialUniformBuffer.name > 0)
            glBindBufferBase(GL_UNIFORM_BUFFER, renderGroup.materialUniformBuffer.bindingPoint, renderGroup.materialUniformBuffer.name);
        for(auto &&buffer : renderGroup.texLayersIndexBuffers){
            glBindBufferBase(GL_UNIFORM_BUFFER, buffer.bindingPoint, buffer.name);
        }
        (this->*DrawFunction)(renderGroup);
    }
}

Renderer::Renderer(){
    version = RenderCapabilities::GetAPIVersion();
    objectsCountToGroup = RenderCapabilities::GetMaxTextureArrayLayers();
    for(GLuint i = 0; i < maxBindingPoints; i++){
        availableBindingPoints.emplace(i, true);
    }

    SetDrawFunction();
}

int Renderer::GetDrawGroupsCount(){
    return renderGroups.size();
}
