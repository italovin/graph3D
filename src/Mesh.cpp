#include "Mesh.hpp"

Mesh::~Mesh(){ 
}

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
std::vector<T> &&data){
    std::div_t verticesDiv = std::div(data.size(), MeshAttribute::ScalarElementsCount(format));
    if(verticesDiv.rem > 0 || (verticesCount > 0 && verticesCount != verticesDiv.quot)){
        return false;
    } else if(verticesCount == 0){
        verticesCount = verticesDiv.quot;
    }
    MeshAttribute meshAttribute;
    meshAttribute.name = name;
    meshAttribute.type = type;
    meshAttribute.format = format;
    meshAttribute.normalized = normalized;
    int totalDataSize = MeshAttribute::AttributeDataSize(type, format)*verticesCount;
    attributesData.emplace_back(std::move(data), totalDataSize, meshAttribute);
    layout.attributes.push_back(meshAttribute);
    return true;
}

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
const std::vector<T> &data){
    return PushAttributeBase(name, format, type, normalized, std::vector<T>(data));
}

void Mesh::SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology){
    SetIndices(std::vector<unsigned int>(indices), topology);
}

void Mesh::SetIndices(std::vector<unsigned int> &&indices, MeshTopology topology){
    this->indices = std::move(indices);
    this->topology = topology;
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<float> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Float, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<float> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::Float, normalized, std::move(data));
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<int> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Int, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<int> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::Int, normalized, std::move(data));
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned int> &data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedInt, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<unsigned int> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedInt, normalized, std::move(data));
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<char> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Byte, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<char> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::Byte, normalized, std::move(data));
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned char> &data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedByte, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<unsigned char> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedByte, normalized, std::move(data));
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<short> &data){
    return PushAttributeBase(name, format, MeshAttributeType::Short, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<short> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::Short, normalized, std::move(data));
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned short> &data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedShort, normalized, data);
}

bool Mesh::PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, std::vector<unsigned short> &&data){
    return PushAttributeBase(name, format, MeshAttributeType::UnsignedShort, normalized, std::move(data));
}

int Mesh::GetAttributesCount() const{
    return attributesData.size();
}

std::vector<int> Mesh::GetAttributesSizes() const{
    std::vector<int> sizes;
    for(auto &&attributeData : attributesData){
        sizes.emplace_back(attributeData.attribute.AttributeDataSize());
    }
    return sizes;
}

std::vector<int> Mesh::GetAttributesDatasSizes() const{
    std::vector<int> datasSizes;
    for(auto &&attributeData : attributesData){
        datasSizes.emplace_back(attributeData.dataSize);
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