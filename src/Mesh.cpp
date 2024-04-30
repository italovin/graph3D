#include "Mesh.hpp"

Mesh::~Mesh(){ 
}

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
const std::vector<T> &data){
    MeshAttribute meshAttribute = { .format = format};
    std::div_t verticesDiv = std::div(data.size(), meshAttribute.ScalarElementsCount());
    if(verticesDiv.rem > 0 || (verticesCount > 0 && verticesCount != verticesDiv.quot)){
        return false;
    } else if(verticesCount == 0){
        verticesCount = verticesDiv.quot;
    }
    meshAttribute.name = name;
    meshAttribute.type = type;
    meshAttribute.normalized = normalized;
    int totalDataSize = meshAttribute.AttributeDataSize()*verticesCount;
    MeshAttributeData meshAttributeData;
    meshAttributeData.data = data;
    meshAttributeData.dataSize = totalDataSize;
    meshAttributeData.attribute = meshAttribute;
    attributesData.push_back(meshAttributeData);
    layout.attributes.push_back(meshAttribute);
    return true;
}

void Mesh::SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology){
    this->indices = indices;
    this->topology = topology;
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<float> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Float, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<int> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Int, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned int> &data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedInt, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<char> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Byte, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned char> &data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedByte, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<short> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Short, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned short> &data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedShort, normalized, data);
}

int Mesh::GetAttributesCount() const{
    return attributesData.size();
}

std::vector<int> Mesh::GetAttributesDatasSizes(){
    std::vector<int> datasSizes;
    for(auto &&attributeData : attributesData){
        datasSizes.push_back(attributeData.dataSize);
    }
    return datasSizes;
}

const std::vector<MeshAttributeData> &Mesh::GetAttributesDatas(){
    return attributesData;
}

int Mesh::GetTotalAttributesDataSize(){
    int size = 0;
    for (auto &&attributeData : attributesData)
    {
        size += attributeData.dataSize;
    }
    return size;
}

const std::vector<unsigned int> &Mesh::GetIndices(){
    return indices;
}

int Mesh::GetIndicesCount() const{
    return indices.size();
}

int Mesh::GetIndicesSize() const{
    return sizeof(unsigned int)*GetIndicesCount();
}

const MeshLayout &Mesh::GetLayout() const{
    return layout;
}

MeshTopology Mesh::GetTopology() const{
    return topology;
}