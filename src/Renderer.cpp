#include "Renderer.hpp"

GLenum Renderer::GetDrawMode(MeshTopology topology){
    switch(topology){
        case MeshTopology::Lines: return GL_LINES;
        case MeshTopology::LineStrip: return GL_LINE_STRIP;
        case MeshTopology::Triangles: return GL_TRIANGLES;
        default: return GL_TRIANGLES;
    }
}

Renderer::Batch Renderer::CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides, 
unsigned int indicesBufferSize, MeshTopology topology, const ShaderProgram &shader,
const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    VertexArray newVao = VertexArray();
    std::vector<GLuint> newBuffers = std::vector<GLuint>(buffersSizes.size());
    glCreateBuffers(buffersSizes.size(), newBuffers.data());
    Batch newBatch;
    for(int i = 0; i < buffersSizes.size(); i++){
        newBatch.buffers.emplace_back(newBuffers[i], buffersSizes[i], strides[i], i);
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
    newBatch.indicesOffsetInBuffer = std::vector<int>(indicesCounts.size(), 0);
    batches.push_back(newBatch);
    return newBatch;
}

void Renderer::StartBatch(Batch &batch){
    for(auto &&buffer : batch.buffers){
        glNamedBufferStorage(buffer.name, buffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(batch.vao.GetHandle(), buffer.bindingPoint, buffer.name, 0, buffer.stride);
    }
    glNamedBufferStorage(batch.indiceBuffer.name, batch.indiceBuffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(batch.vao.GetHandle(), batch.indiceBuffer.name);
}

void Renderer::SetupBatchLayout(Batch &batch, MeshLayout &layout){
    int bindingPoint = 0;
    int locationsOffset = 0;
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
        for(int i = 0; i < locations; i++){
            glVertexArrayAttribFormat(batch.vao.GetHandle(), locationsOffset + i, attribute.ScalarElementsCount(), type, normalized, 0);
            glEnableVertexArrayAttrib(batch.vao.GetHandle(), locationsOffset + i);
            glVertexArrayAttribBinding(batch.vao.GetHandle(), locationsOffset + i, bindingPoint);
        }
        locationsOffset += locations;
        bindingPoint++;
    }
}

void Renderer::BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas){
    int index = 0;
    for(auto &&buffer : batch.buffers){
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

void Renderer::BufferIndices(Batch &batch, unsigned int offset, const std::vector<unsigned int> &indices){
    glNamedBufferSubData(batch.indiceBuffer.name, offset, sizeof(unsigned int)*indices.size(), indices.data());
}

void Renderer::SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    batch.indicesCounts = indicesCounts;
    batch.baseVertices = baseVertices;
}

void Renderer::Prepare(std::vector<MeshRenderer> &&meshRenderers){
    auto comparationFunction = [](const MeshRenderer &a, const MeshRenderer &b){
        return a.GetShader().GetHandle() < b.GetShader().GetHandle();
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
            return (m.GetMesh().GetLayout() == group[0].GetMesh().GetLayout())
            && (m.GetMesh().GetTopology() == group[0].GetMesh().GetTopology());  
        })){
            int renderersCount = group.size();
            int attributesCount = group[0].GetMesh().GetAttributesCount();
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
                const Mesh& mesh = renderer.GetMesh();
                int a = sizeof(mesh);
                indicesSize += mesh.GetIndicesSize();
                indicesOffsets.push_back(indicesOffset);
                indicesOffset += indicesSize;
                indicesCounts.push_back(mesh.GetIndicesCount());
                baseVertices.push_back(baseVertex);
                baseVertex += mesh.GetIndicesCount();

                std::vector<int> attributesDataSizes = mesh.GetAttributesDatasSizes();
                std::vector<int> attributesSizes = mesh.GetAttributesSizes();
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
            Batch createdBatch = CreateBatch(buffersSizes, strides, indicesSize, auxGlobalRenderer.GetMesh().GetTopology(), auxGlobalRenderer.GetShader(),
            indicesCounts, baseVertices);
            StartBatch(createdBatch);
            
            for(int i = 0; i < group.size(); i++){
                auto offsets = dataOffsetsMatrix[i];
                Mesh mesh = group[i].GetMesh();
                BufferSubData(createdBatch, offsets, mesh.GetAttributesDatas());
                BufferIndices(createdBatch, indicesOffsets[i], mesh.GetIndices());
            }
            MeshLayout layout = auxGlobalRenderer.GetMesh().GetLayout();
            SetupBatchLayout(createdBatch, layout);
        }
    }
}

void Renderer::Draw(){
    for(auto &&batch : batches){
        batch.shader.Use();
        batch.vao.Bind();
        glMultiDrawElementsBaseVertex(batch.mode, batch.indicesCounts.data(), 
        GL_UNSIGNED_INT, reinterpret_cast<GLvoid **>(batch.indicesOffsetInBuffer.data()), 
        batch.indicesCounts.size(), batch.baseVertices.data());
    }
}

int Renderer::GetBatchesCount(){
    return batches.size();
}