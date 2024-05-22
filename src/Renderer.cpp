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

void Renderer::AuxBindEboFunction(GLuint ebo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

Renderer::Batch &Renderer::CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides,
                                       unsigned int indicesBufferSize, MeshTopology topology, MeshIndexType type, const Shader &shader,
                                       const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices)
{
    VertexArray newVao = VertexArray();
    std::vector<GLuint> newBuffers = std::vector<GLuint>(buffersSizes.size());
    glCreateBuffers(buffersSizes.size(), newBuffers.data());
    Batch newBatch;
    for(int i = 0; i < buffersSizes.size(); i++){
        newBatch.attributesBuffers.emplace_back(newBuffers[i], buffersSizes[i], strides[i], i);
    }
    GLuint newIndicesBuffer;
    glCreateBuffers(1, &newIndicesBuffer);
    Buffer indicesBuffer(newIndicesBuffer, indicesBufferSize, 0, 0);
    newBatch.vao = newVao;
    newBatch.indiceBuffer = indicesBuffer;
    newBatch.indicesCounts = indicesCounts;
    newBatch.baseVertices = baseVertices;
    newBatch.topology = topology;
    newBatch.shader = shader;
    newBatch.mode = GetDrawMode(topology);
    newBatch.indicesType = GetIndicesType(type);
    newBatch.indicesOffsetInBuffer = std::vector<int>(indicesCounts.size(), 0);
    batches.push_back(newBatch);
    return batches.back();
}

Renderer::Batch &Renderer::CreateBatchWithMVP(const std::vector<unsigned int> &attributesBuffersSizes, const std::vector<int> &strides, 
unsigned int indicesBufferSize, unsigned int mvpBufferSize, unsigned int modelsIndicesBufferSize,
const std::vector<std::reference_wrapper<TransformComponent>> &transforms,
MeshTopology topology, MeshIndexType type, const Shader &shader,
const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    VertexArray newVao = VertexArray();
    int attributesCount = attributesBuffersSizes.size();
    std::vector<GLuint> newBuffers = std::vector<GLuint>(attributesCount);
    glCreateBuffers(attributesCount, newBuffers.data());
    Batch newBatch;
    for(int i = 0; i < attributesCount; i++){
        newBatch.attributesBuffers.emplace_back(newBuffers[i], attributesBuffersSizes[i], strides[i], i);
    }
    GLuint newIndicesBuffer;
    glCreateBuffers(1, &newIndicesBuffer);
    Buffer indicesBuffer(newIndicesBuffer, indicesBufferSize, 0, 0);

    GLuint newModelsIndicesBuffer;
    glCreateBuffers(1, &newModelsIndicesBuffer);
    Buffer modelsIndicesBuffer(newModelsIndicesBuffer, modelsIndicesBufferSize, sizeof(unsigned short), attributesCount);

    GLuint newMVPUniformBuffer;
    glCreateBuffers(1, &newMVPUniformBuffer);
    Buffer MVPUniformBuffer(newMVPUniformBuffer, mvpBufferSize, 0, this->mvpDefaultBindingPoint);
    newBatch.vao = newVao;
    newBatch.indiceBuffer = indicesBuffer;
    newBatch.indicesCounts = indicesCounts;
    newBatch.baseVertices = baseVertices;
    newBatch.topology = topology;
    newBatch.shader = shader;
    newBatch.mode = GetDrawMode(topology);
    newBatch.indicesType = GetIndicesType(type);
    newBatch.indicesOffsetInBuffer = std::vector<int>(indicesCounts.size(), 0);
    newBatch.modelsIndicesBuffer = modelsIndicesBuffer;
    newBatch.mvpUniformBuffer = MVPUniformBuffer;
    newBatch.transforms = transforms;
    newBatch.mvps = std::vector<glm::mat4>(transforms.size());
    batches.push_back(newBatch);
    return batches.back();
}

void Renderer::StartBatch(Batch &batch){
    for(auto &&buffer : batch.attributesBuffers){
        glNamedBufferStorage(buffer.name, buffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(batch.vao.GetHandle(), buffer.bindingPoint, buffer.name, 0, buffer.stride);
    }
    glNamedBufferStorage(batch.indiceBuffer.name, batch.indiceBuffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(batch.vao.GetHandle(), batch.indiceBuffer.name);
}

void Renderer::StartModelsIndicesBuffer(Batch &batch){
    glNamedBufferStorage(batch.modelsIndicesBuffer.name, batch.modelsIndicesBuffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayVertexBuffer(batch.vao.GetHandle(), batch.modelsIndicesBuffer.bindingPoint, batch.modelsIndicesBuffer.name, 0, batch.modelsIndicesBuffer.stride);
}

void Renderer::StartMVPBuffer(Batch &batch){
    glNamedBufferStorage(batch.mvpUniformBuffer.name, batch.mvpUniformBuffer.bufferSize, batch.mvps.data(), GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, batch.mvpUniformBuffer.bindingPoint, batch.mvpUniformBuffer.name);
}

void Renderer::SetupBatchLayout(Batch &batch, MeshLayout &layout){
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
                glVertexArrayAttribIFormat(batch.vao.GetHandle(), i, attribute.ScalarElementsCount(), type, 0);
            } else{
                glVertexArrayAttribFormat(batch.vao.GetHandle(), i, attribute.ScalarElementsCount(), type, normalized, 0);
            }
            glEnableVertexArrayAttrib(batch.vao.GetHandle(), i);
            glVertexArrayAttribBinding(batch.vao.GetHandle(), i, bindingPoint);
        }
        bindingPoint++;
    }
}

void Renderer::SetupModelsIndicesAttribute(Batch &batch)
{
    glVertexArrayAttribIFormat(batch.vao.GetHandle(), modelsIndicesDefaultLocation, 1, GL_UNSIGNED_SHORT, 0);
    glEnableVertexArrayAttrib(batch.vao.GetHandle(), modelsIndicesDefaultLocation);
    glVertexArrayAttribBinding(batch.vao.GetHandle(), modelsIndicesDefaultLocation, batch.attributesBuffers.size());
}

void Renderer::BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas){
    int index = 0;
    for(auto &&buffer : batch.attributesBuffers){
        switch(attributesDatas[index].attribute.type){
            case MeshAttributeType::Float:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<float>>(attributesDatas[index].data).data()); break;
            case MeshAttributeType::Int:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<int>>(attributesDatas[index].data).data()); break;
            case MeshAttributeType::UnsignedInt:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<unsigned int>>(attributesDatas[index].data).data()); break;
            case MeshAttributeType::Byte:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<char>>(attributesDatas[index].data).data()); break;
            case MeshAttributeType::UnsignedByte:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<unsigned char>>(attributesDatas[index].data).data()); break;
            case MeshAttributeType::Short:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<short>>(attributesDatas[index].data).data()); break;
            case MeshAttributeType::UnsignedShort:
            glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, std::get<std::vector<unsigned short>>(attributesDatas[index].data).data()); break;
        }
        index++;
    }
}

void Renderer::BufferIndices(Batch &batch, unsigned int offset, const MeshIndexData &indicesData){
    if(indicesData.type == MeshIndexType::UnsignedInt)
        glNamedBufferSubData(batch.indiceBuffer.name, offset, sizeof(unsigned int)*std::get<std::vector<unsigned int>>(indicesData.indices).size(), std::get<std::vector<unsigned int>>(indicesData.indices).data());
    else if(indicesData.type == MeshIndexType::UnsignedShort)
        glNamedBufferSubData(batch.indiceBuffer.name, offset, sizeof(unsigned short)*std::get<std::vector<unsigned short>>(indicesData.indices).size(), std::get<std::vector<unsigned short>>(indicesData.indices).data());
}

void Renderer::BufferModelsIndices(Batch &batch, unsigned int offset, int index, unsigned int count)
{
    std::vector<unsigned short> modelIndexData(count, index);
    glNamedBufferSubData(batch.modelsIndicesBuffer.name, offset, sizeof(unsigned short)*modelIndexData.size(), modelIndexData.data());
}

void Renderer::BufferSubDataMVPs(Batch &batch){
    glNamedBufferSubData(batch.mvpUniformBuffer.name, 0, sizeof(glm::mat4)*batch.mvps.size(), batch.mvps.data());
}

void Renderer::SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    batch.indicesCounts = indicesCounts;
    batch.baseVertices = baseVertices;
}

/*void Renderer::Prepare(std::vector<MeshRenderer> &&meshRenderers){
    auto comparationFunction = [](const MeshRenderer &a, const MeshRenderer &b){
        return a.GetShader()->GetHandle() < b.GetShader()->GetHandle();
    };
    std::vector<MeshRenderer> _meshRenderers(std::move(meshRenderers));
    std::sort(_meshRenderers.begin(), _meshRenderers.end(), comparationFunction);
    std::vector<std::vector<MeshRenderer>> meshRenderersGroups;

    decltype(_meshRenderers.end()) upper;

    for(auto lower = _meshRenderers.begin(); lower != _meshRenderers.end(); lower = upper)
    {
        // get the upper position of all elements with the same ID
        upper = std::upper_bound(_meshRenderers.begin(), _meshRenderers.end(), *lower, comparationFunction);

        // add those elements as a group to the output vector
        meshRenderersGroups.emplace_back(lower, upper);
    }
    for (auto &&group : meshRenderersGroups)
    {
        if(std::all_of(group.begin(), group.end(), [&](MeshRenderer m){
            return (m.GetMesh()->GetLayout() == group[0].GetMesh()->GetLayout())
            && (m.GetMesh()->GetTopology() == group[0].GetMesh()->GetTopology());  
        })){
            int renderersCount = group.size();
            int attributesCount = group[0].GetMesh()->GetAttributesCount();
            std::vector<unsigned int> buffersSizes = std::vector<unsigned int>(attributesCount);
            std::vector<int> strides = std::vector<int>(attributesCount);
            std::vector<std::vector<unsigned int>> dataOffsetsMatrix(renderersCount);
            std::vector<unsigned int> indicesOffsets;
            indicesOffsets.reserve(renderersCount);
            std::vector<int> baseVertices;
            baseVertices.reserve(renderersCount);
            std::vector<int> indicesCounts;
            indicesCounts.reserve(renderersCount);

            //Temp auxiliary variables
            int indicesSize = 0;
            int rendererIndex = 0;
            int indicesOffset = 0;
            int baseVertex = 0;
            std::vector<int> dataOffsets = std::vector<int>(attributesCount);
            for(auto &&renderer : group){
                auto& mesh = renderer.GetMesh();
                int a = sizeof(mesh);
                indicesSize += mesh->GetIndicesSize();
                indicesOffsets.push_back(indicesOffset);
                indicesOffset += indicesSize;
                indicesCounts.push_back(mesh->GetIndicesCount());
                baseVertices.push_back(baseVertex);
                baseVertex += mesh->GetIndicesCount();

                std::vector<int> attributesDataSizes = mesh->GetAttributesDatasSizes();
                std::vector<int> attributesSizes = mesh->GetAttributesSizes();
                for(int i = 0; i < buffersSizes.size(); i++){
                    if(rendererIndex == 0){
                        strides[i] = attributesSizes[i];
                    }
                    buffersSizes[i] += attributesDataSizes[i];
                    dataOffsetsMatrix[rendererIndex].emplace_back(dataOffsets[i]);
                    dataOffsets[i] += attributesDataSizes[i];
                }
                rendererIndex++;
            }
            const MeshRenderer& auxGlobalRenderer = group[0];
            Batch createdBatch = CreateBatch(buffersSizes, strides, indicesSize, auxGlobalRenderer.GetMesh()->GetTopology(), auxGlobalRenderer.GetMesh()->GetIndicesType(),
            *(auxGlobalRenderer.GetShader()),
            indicesCounts, baseVertices);
            StartBatch(createdBatch);
            
            for(int i = 0; i < group.size(); i++){
                auto offsets = dataOffsetsMatrix[i];
                auto& mesh = group[i].GetMesh();
                BufferSubData(createdBatch, offsets, mesh->GetAttributesDatas());
                BufferIndices(createdBatch, indicesOffsets[i], mesh->GetIndices());
            }
            MeshLayout layout = auxGlobalRenderer.GetMesh()->GetLayout();
            SetupBatchLayout(createdBatch, layout);
        }
    }
}*/

void Renderer::Prepare(entt::registry &registry){
    if(camera == nullptr){
        return;
    }
    auto view = registry.view<MeshRendererComponent, TransformComponent>();
   
    std::vector<std::pair<std::reference_wrapper<MeshRendererComponent>,
    std::reference_wrapper<TransformComponent>>> componentsPairs;
    componentsPairs.reserve(view.size_hint());
    for(auto entity : view){
        auto &meshRenderer = view.get<MeshRendererComponent>(entity);
        auto &transform = view.get<TransformComponent>(entity);
        componentsPairs.emplace_back(meshRenderer, transform);
    }
    
    auto worldObjectsGroups = boolinq::from(componentsPairs)
    .groupBy([](const std::pair<std::reference_wrapper<MeshRendererComponent>,
    std::reference_wrapper<TransformComponent>> &pair){
        return pair.first.get().shader->resourceHandle;
    }).toStdVector();

    for (auto &&groupLinq : worldObjectsGroups)
    {
        auto group = groupLinq.second.toStdVector();
        if(std::all_of(group.begin(), group.end(), [&](std::pair<std::reference_wrapper<MeshRendererComponent>,
    std::reference_wrapper<TransformComponent>> pair){
            return (pair.first.get().mesh->GetLayout() == group[0].first.get().mesh->GetLayout())
            && (pair.first.get().mesh->GetTopology() == group[0].first.get().mesh->GetTopology());  
        })){
            int worldObjectsCount = group.size();
            auto auxGlobalRenderer = group[0].first.get();
            int attributesCount = auxGlobalRenderer.mesh->GetAttributesCount();
            std::vector<unsigned int> buffersSizes(attributesCount);
            std::vector<int> strides(attributesCount);
            std::vector<std::vector<unsigned int>> dataOffsetsMatrix(worldObjectsCount);
            std::vector<unsigned int> indicesOffsets;
            indicesOffsets.reserve(worldObjectsCount);
            std::vector<int> baseVertices;
            baseVertices.reserve(worldObjectsCount);
            std::vector<int> indicesCounts;
            indicesCounts.reserve(worldObjectsCount);
            std::vector<unsigned int> modelsIndicesOffsets;
            modelsIndicesOffsets.reserve(worldObjectsCount);
            std::vector<int> modelsIndicesCounts;
            modelsIndicesCounts.reserve(worldObjectsCount);

            //Temp auxiliary variables
            int indicesSize = 0;
            int verticesCountSize = 0;
            int rendererIndex = 0;
            int indicesOffset = 0;
            int baseVertex = 0;
            int modelsIndicesOffset = 0;
            std::vector<int> dataOffsets(attributesCount);
            std::vector<glm::mat4> mvps;
            mvps.reserve(worldObjectsCount);
            std::vector<std::reference_wrapper<TransformComponent>> transforms;
            transforms.reserve(worldObjectsCount);
            for(auto &&worldObject : group){
                auto& mesh = worldObject.first.get().mesh;
                indicesSize += mesh->GetIndicesSize();
                indicesOffsets.emplace_back(indicesOffset);
                indicesOffset += indicesSize;
                indicesCounts.emplace_back(mesh->GetIndicesCount());

                verticesCountSize += sizeof(unsigned short)*mesh->GetVerticesCount();
                modelsIndicesOffsets.emplace_back(modelsIndicesOffset);
                modelsIndicesOffset += verticesCountSize;
                modelsIndicesCounts.emplace_back(mesh->GetVerticesCount());

                baseVertices.emplace_back(baseVertex);
                baseVertex += mesh->GetIndicesCount();

                std::vector<int> attributesDataSizes = mesh->GetAttributesDatasSizes();
                std::vector<int> attributesSizes = mesh->GetAttributesSizes();
                for(int i = 0; i < buffersSizes.size(); i++){
                    if(rendererIndex == 0){
                        strides[i] = attributesSizes[i];
                    }
                    buffersSizes[i] += attributesDataSizes[i];
                    dataOffsetsMatrix[rendererIndex].emplace_back(dataOffsets[i]);
                    dataOffsets[i] += attributesDataSizes[i];
                }
                auto& objectTransform = worldObject.second;
                transforms.emplace_back(objectTransform);
                glm::mat4 model = glm::mat4(1.0f);
                glm::mat4 scl = glm::scale(model, objectTransform.get().scale);
                glm::mat4 rot = glm::mat4_cast(objectTransform.get().rotation);
                glm::mat4 trn = glm::translate(model, objectTransform.get().position);
                model = trn*rot*scl;
                mvps.emplace_back(projectionMatrix * camera->GetViewMatrix() * model);
                rendererIndex++;
            }
            auto auxGlobalMesh = auxGlobalRenderer.mesh;
            Batch &createdBatch = CreateBatchWithMVP(buffersSizes, strides, indicesSize, 
            sizeof(glm::mat4)*mvps.size(), verticesCountSize,
            transforms, auxGlobalMesh->GetTopology(), auxGlobalMesh->GetIndicesType(),
            *(auxGlobalRenderer.shader), indicesCounts, baseVertices);
            createdBatch.mvps = mvps;
            StartBatch(createdBatch);
            StartMVPBuffer(createdBatch);
            StartModelsIndicesBuffer(createdBatch);
            for(int i = 0; i < group.size(); i++){
                auto offsets = dataOffsetsMatrix[i];
                auto& mesh = group[i].first.get().mesh;
                BufferSubData(createdBatch, offsets, mesh->GetAttributesDatas());
                BufferIndices(createdBatch, indicesOffsets[i], mesh->GetIndices());
                BufferModelsIndices(createdBatch, modelsIndicesOffsets[i], i, modelsIndicesCounts[i]);
            }
            MeshLayout layout = auxGlobalMesh->GetLayout();
            SetupBatchLayout(createdBatch, layout);
            SetupModelsIndicesAttribute(createdBatch);
        }
    }
}

void Renderer::SetupDrawEnvironment(int version)
{
    if(version < 450)
        auxBindEBOFunction = [](GLuint ebo){glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);};
}

void Renderer::SetMVPBindingPoint(int bindingPoint)
{
    this->mvpDefaultBindingPoint = bindingPoint;
}

void Renderer::SetModelsIndicesLocation(int location){
    this->modelsIndicesDefaultLocation = location;
}

void Renderer::SetProjection(const glm::mat4 &projectionMatrix){
    this->projectionMatrix = projectionMatrix;
}

void Renderer::SetMainCamera(Camera *mainCamera){
    this->camera = mainCamera;
}

void Renderer::Draw(){
    for(auto &&batch : batches){
        for(int i = 0; i < batch.transforms.size(); i++){
            auto& transform = batch.transforms[i];
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 scl = glm::scale(model, transform.get().scale);
            glm::mat4 rot = glm::mat4_cast(transform.get().rotation);
            glm::mat4 trn = glm::translate(model, transform.get().position);
            model = trn*rot*scl;
            batch.mvps[i] = projectionMatrix * camera->GetViewMatrix() * model;
        }
        BufferSubDataMVPs(batch);
        glBindBufferBase(GL_UNIFORM_BUFFER, batch.mvpUniformBuffer.bindingPoint, batch.mvpUniformBuffer.name);
        batch.shader.Use();
        batch.vao.Bind();

        //For older GL, this function calls a bind for batch EBO, this fix the retrieve of EBO info
        auxBindEBOFunction(batch.indiceBuffer.name);

        glMultiDrawElementsBaseVertex(batch.mode, batch.indicesCounts.data(), 
        batch.indicesType, reinterpret_cast<GLvoid **>(batch.indicesOffsetInBuffer.data()), 
        batch.indicesCounts.size(), batch.baseVertices.data());
    }
}

int Renderer::GetBatchesCount(){
    return batches.size();
}