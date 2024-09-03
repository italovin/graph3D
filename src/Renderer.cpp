#include "Renderer.hpp"

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
    // This index is equals last attribute index plus one
    int objectsIndexerBindingIndex = renderGroup.attributesBuffers.size();
    glVertexArrayAttribIFormat(renderGroup.vao.GetHandle(), this->objectsIndexerDefaultLocation, 1, GL_UNSIGNED_SHORT, 0);
    glEnableVertexArrayAttrib(renderGroup.vao.GetHandle(), this->objectsIndexerDefaultLocation);
    glVertexArrayAttribBinding(renderGroup.vao.GetHandle(), this->objectsIndexerDefaultLocation, objectsIndexerBindingIndex);
    glVertexArrayBindingDivisor(renderGroup.vao.GetHandle(), objectsIndexerBindingIndex, 1);
}

void Renderer::BindRenderGroupAttributesBuffers(RenderGroup &renderGroup)
{
    std::vector<GLuint> buffers;
    std::vector<GLintptr> offsets;
    std::vector<GLsizei> strides;
    //Attributes buffer + extra attribute for index the models (Compatibility for no gl_DrawID)
    GLsizei buffersCount = renderGroup.attributesBuffers.size() + 1;
    buffers.reserve(buffersCount);
    offsets.reserve(buffersCount);
    strides.reserve(buffersCount);
    for(auto &&buffer : renderGroup.attributesBuffers){
        buffers.emplace_back(buffer.name);
        offsets.emplace_back(0);
        strides.emplace_back(buffer.stride);
    }
    buffers.emplace_back(renderGroup.objectIndexerBuffer.name);
    offsets.emplace_back(0);
    strides.emplace_back(renderGroup.objectIndexerBuffer.stride);
    glVertexArrayVertexBuffers(renderGroup.vao.GetHandle(), 0, buffersCount, buffers.data(), offsets.data(), strides.data());
    glVertexArrayElementBuffer(renderGroup.vao.GetHandle(), renderGroup.indicesBuffer.name);
}

void Renderer::PrepareRenderGroups(entt::registry &registry){
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
        Shader shader;
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
    for(auto &&x: componentsPairs){
        shaderMap[x.first.get().material->GetShader()->resourceHandle].emplace_back(std::move(x));
    }
    std::vector<std::vector<Renderable>> result;
    for(auto &&x: shaderMap){
        result.emplace_back(std::move(x.second));
    }
    for(auto &&group : result){
        ShaderGroup shaderGroup;
        shaderGroup.shader = *(group[0].first.get().material->GetShader());
        std::unordered_map<ResourceHandle, std::vector<Renderable>> groupMap;
        for(auto &&x : group){
            groupMap[x.first.get().mesh->resourceHandle].emplace_back(std::move(x));
        }
        std::vector<std::vector<Renderable>> groupByMeshes;
        for(auto &&x : groupMap){
            if(x.second.size() >= 2){
                shaderGroup.instancesGroups.emplace_back(std::move(x.second));
            } else {
                for(auto &&y : x.second){
                    shaderGroup.batchGroup.emplace_back(std::move(y));
                }
            }
        }
        shaderGroups.emplace_back(std::move(shaderGroup));
    }
    for(auto &&shaderGroup : shaderGroups){
        RenderGroup renderGroup;
        renderGroup.vao = VertexArray();
        renderGroup.shader = shaderGroup.shader;
        MeshLayout layout;
        std::vector<DrawElementsIndirectCommand> commands;

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
        for(int i = 0; i < attributesBatchedChunks.size(); i++){
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
            }
        }

        MeshIndexData indicesBatchedChunk;
        indicesBatchedChunk.type = meshGlobalIndicesType;

        switch(indicesBatchedChunk.type){
            case MeshIndexType::UnsignedInt:
            indicesBatchedChunk.indices = std::vector<unsigned int>(); break;
            case MeshIndexType::UnsignedShort:
            indicesBatchedChunk.indices = std::vector<unsigned short>(); break;
        }

        //Temp auxiliary variables
        int rendererIndex = 0;
        int baseVertex = 0;
        unsigned int firstIndex = 0;
        unsigned int baseInstance = 0;

        renderGroup.mvps.reserve(objectsCount);
        renderGroup.transforms.reserve(objectsCount);

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
                }
            }

            unsigned int instanceCount = 1;

            DrawElementsIndirectCommand meshCmd = {
                .count = mesh->GetIndicesCount(),
                .instanceCount = instanceCount,
                .firstIndex = firstIndex,    // First in the index array
                .baseVertex = baseVertex,    // First in the vertex array
                .baseInstance = baseInstance
            };
            commands.emplace_back(std::move(meshCmd));

            //Base vertex is offset of vertices, not of indices
            baseVertex += mesh->GetVerticesCount();
            firstIndex += mesh->GetIndicesCount();
            baseInstance += instanceCount;

            auto& objectTransform = object.second;
            renderGroup.transforms.emplace_back(objectTransform);
            renderGroup.mvps.emplace_back(glm::mat4(1.0f));
            rendererIndex++;
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
                }
            }

            DrawElementsIndirectCommand meshCmd = {
                .count = mesh->GetIndicesCount(),
                .instanceCount = instanceCount,
                .firstIndex = firstIndex,    // First in the index array
                .baseVertex = baseVertex,    // First in the vertex array
                .baseInstance = baseInstance
            };
            commands.emplace_back(std::move(meshCmd));

            //Base vertex is offset of vertices, not of indices
            baseVertex += mesh->GetVerticesCount();
            firstIndex += mesh->GetIndicesCount();
            baseInstance += instanceCount;

            for(auto &&object : instanceGroup){
                auto& objectTransform = object.second;
                renderGroup.transforms.emplace_back(objectTransform);
                renderGroup.mvps.emplace_back(glm::mat4(1.0f));
            }
            rendererIndex++;
        }
        renderGroup.commands = commands;
        std::vector<GLuint> attributesBuffersNames(attributesCount);
        glCreateBuffers(attributesCount, attributesBuffersNames.data());
        std::vector<Buffer> attributesBuffers(attributesCount);
        for(int i = 0; i < attributesBuffers.size(); i++){
            renderGroup.attributesBuffers.emplace_back(attributesBuffersNames[i],
            attributesBatchedChunks[i].dataSize, attributesBatchedChunks[i].attribute.AttributeDataSize(), i);
        }

        GLuint indicesBufferName = 0;
        glCreateBuffers(1, std::addressof(indicesBufferName));
        renderGroup.indicesBuffer.name = indicesBufferName;
        renderGroup.indicesBuffer.bufferSize = indicesBatchedChunk.indicesSize;

        GLuint objectIndexBufferName = 0;
        glCreateBuffers(1, std::addressof(objectIndexBufferName));
        renderGroup.objectIndexerBuffer.name = objectIndexBufferName;
        renderGroup.objectIndexerBuffer.bufferSize = sizeof(unsigned short)*objectsCount;
        renderGroup.objectIndexerBuffer.stride = sizeof(unsigned short);
        renderGroup.objectIndexerBuffer.bindingPoint = attributesCount;

        GLuint mpvsUniformBufferName;
        glCreateBuffers(1, &mpvsUniformBufferName);
        renderGroup.mvpsUniformBuffer.name = mpvsUniformBufferName;
        renderGroup.mvpsUniformBuffer.bufferSize = sizeof(glm::mat4)*renderGroup.mvps.size();
        renderGroup.mvpsUniformBuffer.stride = sizeof(glm::mat4);
        renderGroup.mvpsUniformBuffer.bindingPoint = this->mvpDefaultBindingPoint;

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
            }
            index++;
        }

        switch(indicesBatchedChunk.type){
            case MeshIndexType::UnsignedInt:
            glNamedBufferStorage(renderGroup.indicesBuffer.name, indicesBatchedChunk.indicesSize, std::get<std::vector<unsigned int>>(indicesBatchedChunk.indices).data(), GL_DYNAMIC_STORAGE_BIT); break;
            case MeshIndexType::UnsignedShort:
            glNamedBufferStorage(renderGroup.indicesBuffer.name, indicesBatchedChunk.indicesSize, std::get<std::vector<unsigned short>>(indicesBatchedChunk.indices).data(), GL_DYNAMIC_STORAGE_BIT); break;
        }

        {
            std::vector<unsigned short> objectsIndexer;
            objectsIndexer.reserve(objectsCount);
            for(int i = 0; i < objectsCount; i++){
                objectsIndexer.emplace_back(i);
            }
            glNamedBufferStorage(renderGroup.objectIndexerBuffer.name, sizeof(unsigned short)*objectsIndexer.size(), objectsIndexer.data(), GL_DYNAMIC_STORAGE_BIT);
        }

        glNamedBufferStorage(renderGroup.mvpsUniformBuffer.name, renderGroup.mvpsUniformBuffer.bufferSize, renderGroup.mvps.data(), GL_DYNAMIC_STORAGE_BIT);

        SetRenderGroupLayout(renderGroup, meshGlobalLayout);
        BindRenderGroupAttributesBuffers(renderGroup);

        GLuint drawCmdBuffer = 0;
        glCreateBuffers(1, std::addressof(drawCmdBuffer));
        renderGroup.drawCmdBuffer.name = drawCmdBuffer;
        GLsizeiptr drawCmdBufferSize = sizeof(DrawElementsIndirectCommand) * commands.size();
        renderGroup.drawCmdBuffer.bufferSize = drawCmdBufferSize;
        glNamedBufferStorage(drawCmdBuffer,
                     drawCmdBufferSize,
                     commands.data(),
                     GL_DYNAMIC_STORAGE_BIT);

        renderGroup.drawCmdBuffer.commandsCount = commands.size();
        this->renderGroups.emplace_back(renderGroup);
    }
}

void Renderer::SetMVPBindingPoint(int bindingPoint){
    this->mvpDefaultBindingPoint = bindingPoint;
}

void Renderer::SetObjectsIndexerLocation(int location){
    this->objectsIndexerDefaultLocation = location;
}

void Renderer::SetDrawFunction(){
    DrawFunction = version >= GLApiVersion::V400 ?
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
    for(auto&& command : renderGroup.commands){
        glDrawElementsInstancedBaseVertexBaseInstance(
        renderGroup.mode,
        command.count,
        renderGroup.indicesType,
        reinterpret_cast<GLvoid *>(command.firstIndex*renderGroup.indicesTypeSize),
        command.instanceCount,
        command.baseVertex,
        command.baseInstance
        );
    }
}

void Renderer::SetAPIVersion(GLApiVersion version){
    this->version = version;
    SetDrawFunction();
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
        renderGroup.shader.Use();
        renderGroup.vao.Bind();

        for(int i = 0; i < renderGroup.transforms.size(); i++){
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

        (this->*DrawFunction)(renderGroup);
    }
}

int Renderer::GetDrawGroupsCount(){
    return renderGroups.size();
}
