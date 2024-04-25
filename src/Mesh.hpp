#ifndef MESH_H
#define MESH_H
#include <vector>
#include <string>
#include "ShaderTypes.hpp"

enum class MeshTopology{
    Triangles,
    Lines,
    LineStrip
};

struct MeshAttribute{
    std::string name;
    ShaderDataType type;
    bool normalized;
    bool operator == (const MeshAttribute &attribute){
        return (attribute.type == type) && (attribute.normalized == normalized);
    }
    int LocationsCount() const{
        switch(type){
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool: return 1;
            case ShaderDataType::Mat3: return 3;
            case ShaderDataType::Mat4: return 4;
            default: return 1;
        }
    }
    int ScalarElementsCount() const{
        switch(type){
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Int: return 1;
            case ShaderDataType::Int2: return 2;
            case ShaderDataType::Int3: return 3;
            case ShaderDataType::Int4: return 4;
            case ShaderDataType::Bool: return 1;
            case ShaderDataType::Mat3: return 3*3;
            case ShaderDataType::Mat4: return 4*4;
            default: return 1;
        }
    }
    int AttributeDataSize() const{
        int locations = LocationsCount();
        switch(type){
            case ShaderDataType::Float: return 1 * sizeof(float);
            case ShaderDataType::Float2: return 2 * sizeof(float);
            case ShaderDataType::Float3: return 3 * sizeof(float);
            case ShaderDataType::Float4: return 4 * sizeof(float);
            case ShaderDataType::Mat3: return 3 * 3 * sizeof(float);
            case ShaderDataType::Mat4: return 4 * 4 * sizeof(float);
            case ShaderDataType::Int: return 1 * sizeof(int);
            case ShaderDataType::Int2: return 2 * sizeof(int);
            case ShaderDataType::Int3: return 3 * sizeof(int);
            case ShaderDataType::Int4: return 4 * sizeof(int);
            case ShaderDataType::Bool: return 1 * sizeof(bool);
            default: return 0;
        }
    }
};
struct MeshLayout{
    std::vector<MeshAttribute> attributes;
    bool operator == (const MeshLayout &layout){
        if(attributes.size() == 0 || attributes.size() != layout.attributes.size()){
            return false;
        }
        bool isEqual = true;
        for (int i = 0; i < attributes.size(); i++)
        {
            if(!(attributes[i] == layout.attributes[i])){
                isEqual = false;
                break;
            }
        }
        return isEqual;
    }
};

struct MeshAttributeData{
    std::vector<float> data;
    int dataSize;
    MeshAttribute attribute;
};

class Mesh{
private:
    std::vector<unsigned int> indices;
    MeshTopology topology;
    std::vector<MeshAttributeData> attributesData;
    int verticesCount = 0;
    MeshLayout layout;
public:
    ~Mesh();
    void SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology);
    bool PushAttribute(const std::string &name, ShaderDataType type, bool normalized, const std::vector<float> &data);
    int GetAttributesCount();
    std::vector<int> GetAttributeDatasSizes();
    int GetTotalDataSize();
    int GetIndicesCount();
    int GetIndicesSize();
    MeshLayout GetLayout();
    MeshTopology GetTopology();
    const std::vector<MeshAttributeData> &GetAttributesDatas();
    const std::vector<unsigned int> &GetIndices();
};
#endif