#include "Renderer.hpp"

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

void Renderer::SetRenderGroupLayout(RenderGroup &renderGroup, MeshLayout &layout){
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

void Renderer::BindRenderGroupAttributesBuffers(RenderGroup &renderGroup)
{
    std::vector<GLuint> buffers;
    std::vector<GLintptr> offsets;
    std::vector<GLsizei> strides;
    //Attributes buffer
    GLsizei buffersCount = renderGroup.attributesBuffers.size();
    buffers.reserve(buffersCount);
    offsets.reserve(buffersCount);
    strides.reserve(buffersCount);
    for(auto &&buffer : renderGroup.attributesBuffers){
        buffers.emplace_back(buffer.name);
        offsets.emplace_back(0);
        strides.emplace_back(buffer.stride);
    }
    glVertexArrayVertexBuffers(renderGroup.vao.GetHandle(), 0, buffersCount, buffers.data(), offsets.data(), strides.data());
    glVertexArrayElementBuffer(renderGroup.vao.GetHandle(), renderGroup.indicesBuffer.name);
}

void Renderer::PrepareRenderGroups(entt::registry &registry){
    auto mapBegin = std::chrono::high_resolution_clock::now();
    auto renderableView = registry.view<MeshRendererComponent, TransformComponent>();

    using Renderable = std::pair<std::reference_wrapper<MeshRendererComponent>,
    std::reference_wrapper<TransformComponent>>;
    std::vector<Renderable> componentsPairs;
    componentsPairs.reserve(renderableView.size_hint());
    for(auto entity : renderableView){
        auto &meshRenderer = renderableView.get<MeshRendererComponent>(entity);
        auto &transform = renderableView.get<TransformComponent>(entity);
        componentsPairs.emplace_back(meshRenderer, transform);
    }

    struct ShaderGroup{
        Ref<GL::ShaderGL> shader;
        std::vector<Renderable> batchGroup;
        std::vector<std::vector<Renderable>> instancesGroups;
        std::vector<Renderable> &GetBatchGroup(){
            return batchGroup;
        }
        std::vector<std::vector<Renderable>> &GetInstancesGroups(){
            return instancesGroups;
        }
    };
    std::vector<ShaderGroup> shaderGroups;

    std::unordered_map<ResourceHandle, std::vector<Renderable>> shaderMap;
    std::unordered_map<ShaderCode*, std::vector<Renderable>> shaderCodeMap;
    std::unordered_map<ResourceHandle, GL::ShaderGLResource> shaderCache;
    for(auto &&x: componentsPairs){
        auto shaderCode = x.first.get().material->GetShaderCode();
        if(!shaderCode){
            continue;
        }
        shaderCodeMap[shaderCode.get()].push_back(std::move(x));
    }
    for(auto &&x : shaderCodeMap){
        GL::ShaderGLResource shaderGeneratedGlobal(x.second[0].first.get().material->GetShaderCode()->Generate());
        if(!shaderGeneratedGlobal.object)
            continue;
        std::vector<std::vector<Renderable>> shaderGeneratedGroups;
        const int groupSize = this->objectsCountToGroup;
        shaderGeneratedGroups.reserve(std::ceil(x.second.size()/groupSize));
        for(size_t i = 0; i < x.second.size(); i+=groupSize){
            std::vector<Renderable> vec(x.second.begin() + i, x.second.begin() + std::min(i+groupSize, x.second.size()));
            shaderGeneratedGroups.push_back(std::move(vec));
        }
        for(auto &&group : shaderGeneratedGroups){
            GL::ShaderGLResource shaderGenerated(shaderGeneratedGlobal.object);
            // Setting shader mvp binding point;
            for(auto &&bindingPurpose : x.second[0].first.get().material->GetShaderCode()->GetBindingsPurposes(ShaderStage::Vertex)){
                std::optional<int> binding = AddUBOBindingPurpose(bindingPurpose.second);
                if(!binding.has_value()){
                    // No binding point available
                    return;
                }
                shaderGenerated->SetBlockBinding(bindingPurpose.first, uboBindingsPurposes[bindingPurpose.second]);
            }
            shaderMap[shaderGenerated.resourceHandle].reserve(group.size());
            for(auto &&renderable : group){
                shaderMap[shaderGenerated.resourceHandle].push_back(std::move(renderable));
                shaderCache[shaderGenerated.resourceHandle] = shaderGenerated;
            }
        }
    }
    for(auto &&group : shaderMap){
        ShaderGroup shaderGroup;
        shaderGroup.shader = shaderCache[group.first].object;
        std::unordered_map<Mesh*, std::vector<Renderable>> groupMap;
        int batchSize = 0;
        for(auto &&x : group.second){
            if(groupMap[x.first.get().mesh.object.get()].size() == 0)
                batchSize++;
            else if(groupMap[x.first.get().mesh.object.get()].size() == 1)
                batchSize--; // Reverts when instances are found

            groupMap[x.first.get().mesh.object.get()].push_back(std::move(x));
        }
        shaderGroup.batchGroup.reserve(batchSize);
        for(auto &&x : groupMap){
            if(x.second.size() >= 2){
                shaderGroup.instancesGroups.push_back(std::move(x.second));
            } else {
                for(auto &&y : x.second){
                    shaderGroup.batchGroup.push_back(std::move(y));
                }
            }
        }
        shaderGroups.push_back(std::move(shaderGroup));
    }
    auto mapEnd = std::chrono::high_resolution_clock::now();
    std::cout << "Time to shaders mapping and compiling: " << std::chrono::duration_cast<std::chrono::nanoseconds>(mapEnd-mapBegin).count()/1000 << " (μs)" << std::endl;
    for(auto &&shaderGroup : shaderGroups){
        auto setupBegin = std::chrono::high_resolution_clock::now();
        renderGroups.push_back(RenderGroup());
        RenderGroup& renderGroup = renderGroups.back();
        renderGroup.shader = shaderGroup.shader;
        MeshLayout layout;

        auto& batchGroup = shaderGroup.GetBatchGroup();
        auto& instancesGroups = shaderGroup.GetInstancesGroups();
        size_t objectsCount = batchGroup.size();

        if(!std::all_of(batchGroup.begin(), batchGroup.end(), [&](Renderable pair){
            return (pair.first.get().mesh->GetLayout() == batchGroup[0].first.get().mesh->GetLayout())
            && (pair.first.get().mesh->GetTopology() == batchGroup[0].first.get().mesh->GetTopology()
            && (pair.first.get().mesh->GetIndicesType() == batchGroup[0].first.get().mesh->GetIndicesType()));
        })){
            return;
        }
        for(auto&& instanceGroup : instancesGroups){
            // Adding objects from instancing grouping to object counter
            objectsCount += instanceGroup.size();
            if(!std::all_of(instanceGroup.begin(), instanceGroup.end(), [&](Renderable pair){
            return (pair.first.get().mesh->GetLayout() == instanceGroup[0].first.get().mesh->GetLayout())
            && (pair.first.get().mesh->GetTopology() == instanceGroup[0].first.get().mesh->GetTopology())
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
        } else if(batchGroup.size() > 0 && instancesGroups.size() > 0 && instancesGroups[0].size() > 0){
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
        int textureParametersCounter = 0; // Counter for texture parameters
        int baseVertex = 0;
        unsigned int firstIndex = 0;
        unsigned int baseInstance = 0;

        renderGroup.mvps.reserve(objectsCount);
        renderGroup.transforms.reserve(objectsCount);
        renderGroup.instancesGroups.reserve(instancesGroups.size());

        // Boolean to check if material parameters struct layout is calculated
        bool areParametersMembersSet = false;
        StructArray matParamStructArray;

        // Boolean to check if texture array is initialized
        bool areTexturesArraysInitialized = false;
        // Store max dimensions for build each texture array
        struct MaxTexDimensions{
            size_t maxWidth = 0;
            size_t maxHeight = 0;
        };
        std::vector<MaxTexDimensions> maxTexDimensions;
        if(batchGroup.size() > 0){
            maxTexDimensions = std::vector<MaxTexDimensions>(batchGroup[0].first.get().material->GetMapParameters().size());
        } else if(instancesGroups.size() > 0){
            if(instancesGroups[0].size() > 0)
                maxTexDimensions = std::vector<MaxTexDimensions>(instancesGroups[0][0].first.get().material->GetMapParameters().size());
        }

        for(auto &&object : batchGroup){
            auto texParameters = object.first.get().material->GetMapParameters();
            int texParameterIndexer = 0;
            for(auto &&texParameter : texParameters){
                auto tex = std::get<Ref<Texture>>(texParameter.second.data);
                if(!tex)
                    continue;
                size_t texWidth = tex->GetWidth();
                size_t texHeight = tex->GetHeight();
                if(texWidth > maxTexDimensions[texParameterIndexer].maxWidth){
                    maxTexDimensions[texParameterIndexer].maxWidth = texWidth;
                }
                if(texHeight > maxTexDimensions[texParameterIndexer].maxHeight){
                    maxTexDimensions[texParameterIndexer].maxHeight = texHeight;
                }
                texParameterIndexer++;
            }
        }
        for(auto &&instanceGroup : instancesGroups){
            for(auto &&object : instanceGroup){
                auto texParameters = object.first.get().material->GetMapParameters();
                int texParameterIndexer = 0;
                for(auto &&texParameter : texParameters){
                    auto tex = std::get<Ref<Texture>>(texParameter.second.data);
                    if(!tex)
                        continue;
                    size_t texWidth = tex->GetWidth();
                    size_t texHeight = tex->GetHeight();
                    if(texWidth > maxTexDimensions[texParameterIndexer].maxWidth){
                        maxTexDimensions[texParameterIndexer].maxWidth = texWidth;
                    }
                    if(texHeight > maxTexDimensions[texParameterIndexer].maxHeight){
                        maxTexDimensions[texParameterIndexer].maxHeight = texHeight;
                    }
                    texParameterIndexer++;
                }
            }
        }

        for(auto &&object : batchGroup){

            auto& mesh = object.first.get().mesh;

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
            auto matTexParameters = objectMaterial->GetMapParameters();
            for(auto &&parameter : matTexParameters){
                auto texture = std::get<Ref<Texture>>(parameter.second.data);
                if(!texture){
                    continue;
                }
                if(!areTexturesArraysInitialized){
                    Ref<GL::TextureGL> textureGL = CreateRef<GL::TextureGL>(GL_TEXTURE_2D_ARRAY, texture->GetFormatGLenum(), texture->GetPixelDataTypeGLenum());
                    textureGL->SetupParameters();
                    textureGL->SetupStorage3D(maxTexDimensions[textureParametersCounter].maxWidth, maxTexDimensions[textureParametersCounter].maxHeight, objectsCount);
                    renderGroup.texturesArrays.push_back(GL::TextureGLResource(textureGL));
                    renderGroup.shader->SetInt(parameter.first, textureParametersCounter);
                }
                if(texture->GetPixels().dataType == TexturePixelDataType::UnsignedByte){
                    renderGroup.texturesArrays[textureParametersCounter]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), objectIndex, std::get<std::vector<GLubyte>>(texture->GetPixels().data));
                } else if(texture->GetPixels().dataType == TexturePixelDataType::Float){
                    renderGroup.texturesArrays[textureParametersCounter]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), objectIndex, std::get<std::vector<GLfloat>>(texture->GetPixels().data));
                }
                textureParametersCounter++;
            }
            // Resets indexer for which texture array choose
            textureParametersCounter = 0;
            // Texture array is initialized
            areTexturesArraysInitialized = true;

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
                renderGroup.mvps.push_back(glm::mat4(1.0f));
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
                    textureParametersCounter = 0;
                    areTexturesArraysInitialized = true;
                }
                auto matTexParameters = objectMaterial->GetMapParameters();
                for(auto &&parameter : matTexParameters){
                    auto texture = std::get<Ref<Texture>>(parameter.second.data);
                    if(!texture){
                        continue;
                    }
                    if(!areTexturesArraysInitialized){
                        Ref<GL::TextureGL> textureGL = CreateRef<GL::TextureGL>(GL_TEXTURE_2D_ARRAY, texture->GetFormatGLenum(), texture->GetPixelDataTypeGLenum());
                        textureGL->SetupParameters();
                        textureGL->SetupStorage3D(maxTexDimensions[textureParametersCounter].maxWidth, maxTexDimensions[textureParametersCounter].maxHeight, objectsCount);
                        renderGroup.texturesArrays.push_back(GL::TextureGLResource(textureGL));
                        renderGroup.shader->SetInt(parameter.first, textureParametersCounter);
                    }
                    if(texture->GetPixels().dataType == TexturePixelDataType::UnsignedByte){
                        renderGroup.texturesArrays[textureParametersCounter]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), objectIndex, std::get<std::vector<GLubyte>>(texture->GetPixels().data));
                    } else if(texture->GetPixels().dataType == TexturePixelDataType::Float){
                        renderGroup.texturesArrays[textureParametersCounter]->PushData3DLayer(texture->GetWidth(), texture->GetHeight(), objectIndex, std::get<std::vector<GLfloat>>(texture->GetPixels().data));
                    }
                    textureParametersCounter++;
                }
                // Resets indexer for which texture array choose
                textureParametersCounter = 0;
                // Texture array is initialized
                areTexturesArraysInitialized = true;

                objectIndex++;
            }
            meshIndex++;
        }
        renderGroup.objectsCount = objectsCount;
        renderGroup.materialsStructArray = matParamStructArray;
        std::vector<GLuint> attributesBuffersNames(attributesCount);
        glCreateBuffers(attributesCount, attributesBuffersNames.data());
        std::vector<Buffer> attributesBuffers(attributesCount);
        for(size_t i = 0; i < attributesBuffers.size(); i++){
            renderGroup.attributesBuffers.emplace_back(attributesBuffersNames[i],
            attributesBatchedChunks[i].dataSize, attributesBatchedChunks[i].attribute.AttributeDataSize(), i);
        }

        GLuint indicesBufferName = 0;
        glCreateBuffers(1, std::addressof(indicesBufferName));
        renderGroup.indicesBuffer.name = indicesBufferName;
        renderGroup.indicesBuffer.bufferSize = indicesBatchedChunk.indicesSize;

        GLuint mpvsUniformBufferName = 0;
        glCreateBuffers(1, std::addressof(mpvsUniformBufferName));
        renderGroup.mvpsUniformBuffer.name = mpvsUniformBufferName;
        renderGroup.mvpsUniformBuffer.bufferSize = sizeof(glm::mat4)*renderGroup.mvps.size();
        renderGroup.mvpsUniformBuffer.stride = sizeof(glm::mat4);
        renderGroup.mvpsUniformBuffer.bindingPoint = uboBindingsPurposes["mvps"];

        GLuint materialUniformBufferName = 0;
        glCreateBuffers(1, std::addressof(materialUniformBufferName));
        renderGroup.materialUniformBuffer.name = materialUniformBufferName;
        renderGroup.materialUniformBuffer.bufferSize = renderGroup.materialsStructArray.structSize *renderGroup.materialsStructArray.numStructs;
        renderGroup.materialUniformBuffer.stride = renderGroup.materialsStructArray.structSize;
        renderGroup.materialUniformBuffer.bindingPoint = uboBindingsPurposes["materials"];


        auto setupEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Time to setup batch and instance groups: " << std::chrono::duration_cast<std::chrono::microseconds>(setupEnd-setupBegin).count() << " (μs)" << std::endl;
        auto bufferBegin = std::chrono::high_resolution_clock::now();
        {
            int index = 0;
            for(auto &&buffer : renderGroup.attributesBuffers){
                switch(attributesBatchedChunks[index].attribute.type){
                    case MeshAttributeType::Float:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<float>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::Int:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<int>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::UnsignedInt:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<unsigned int>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::Byte:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<char>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::UnsignedByte:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<unsigned char>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::Short:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<short>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::UnsignedShort:
                    glNamedBufferStorage(buffer.name, attributesBatchedChunks[index].dataSize, std::get<std::vector<unsigned short>>(attributesBatchedChunks[index].data).data(), GL_DYNAMIC_STORAGE_BIT); break;
                    case MeshAttributeType::None: break;
                }
                index++;
            }
        }

        switch(indicesBatchedChunk.type){
            case MeshIndexType::UnsignedInt:
            glNamedBufferStorage(renderGroup.indicesBuffer.name, renderGroup.indicesBuffer.bufferSize, std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).data(), GL_DYNAMIC_STORAGE_BIT); break;
            case MeshIndexType::UnsignedShort:
            glNamedBufferStorage(renderGroup.indicesBuffer.name, renderGroup.indicesBuffer.bufferSize, std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).data(), GL_DYNAMIC_STORAGE_BIT); break;
            case MeshIndexType::None: break;
        }

        // MVP UBO
        glNamedBufferStorage(renderGroup.mvpsUniformBuffer.name, renderGroup.mvpsUniformBuffer.bufferSize, renderGroup.mvps.data(), GL_DYNAMIC_STORAGE_BIT);

        // Material UBO
        if(renderGroup.materialUniformBuffer.bufferSize > 0)
            glNamedBufferStorage(renderGroup.materialUniformBuffer.name, renderGroup.materialUniformBuffer.bufferSize,
            renderGroup.materialsStructArray.GetData().data(), GL_DYNAMIC_STORAGE_BIT);

        SetRenderGroupLayout(renderGroup, meshGlobalLayout);
        BindRenderGroupAttributesBuffers(renderGroup);

        if(isIndirect){
            GLuint drawCmdBufferName = 0;
            glCreateBuffers(1, std::addressof(drawCmdBufferName));
            renderGroup.drawCmdBuffer.name = drawCmdBufferName;
            renderGroup.drawCmdBuffer.bufferSize = sizeof(DrawElementsIndirectCommand) * renderGroup.commands.size();
            glNamedBufferStorage(renderGroup.drawCmdBuffer.name, renderGroup.drawCmdBuffer.bufferSize, renderGroup.commands.data(), GL_DYNAMIC_STORAGE_BIT);
        }


        auto bufferEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Time to buffer data: " << std::chrono::duration_cast<std::chrono::microseconds>(bufferEnd-bufferBegin).count() << " (μs)" << std::endl;

        renderGroup.drawCmdBuffer.commandsCount = renderGroup.commands.size();
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
    Draw(mainCamera, mainCameraTransform);
}

void Renderer::Draw(const CameraComponent &mainCamera, const TransformComponent &mainCameraTransform){
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
        renderGroup.shader->Use();
        renderGroup.vao.Bind();

        for(int i = 0; i < renderGroup.objectsCount; i++){
            auto& transform = renderGroup.transforms[i];
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 scl = glm::scale(model, transform.get().scale);
            glm::mat4 rot = glm::mat4_cast(transform.get().rotation);
            glm::mat4 trn = glm::translate(model, transform.get().position);
            model = trn*rot*scl;
            renderGroup.mvps[i] = mainCameraProjection * mainCameraView * model;

        }

        glBindBufferBase(GL_UNIFORM_BUFFER, renderGroup.mvpsUniformBuffer.bindingPoint, renderGroup.mvpsUniformBuffer.name);
        BufferSubDataMVPs(renderGroup);
        if(renderGroup.materialUniformBuffer.name > 0)
            glBindBufferBase(GL_UNIFORM_BUFFER, renderGroup.materialUniformBuffer.bindingPoint, renderGroup.materialUniformBuffer.name);

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
