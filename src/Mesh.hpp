#ifndef MESH_H
#define MESH_H
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>

enum class MeshTopology{
    Triangles,
    Lines,
    LineStrip
};

//Associated with shader types
enum class MeshDataType{
    Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

struct MeshAttribute{
    std::string name;
    MeshDataType type;
    bool normalized;
    bool operator == (const MeshAttribute &attribute){
        return (attribute.type == type) && (attribute.normalized == normalized);
    }
    int LocationsCount() const{
        switch(type){
            case MeshDataType::Float:
            case MeshDataType::Float2:
            case MeshDataType::Float3:
            case MeshDataType::Float4:
            case MeshDataType::Int:
            case MeshDataType::Int2:
            case MeshDataType::Int3:
            case MeshDataType::Int4:
            case MeshDataType::Bool: return 1;
            case MeshDataType::Mat3: return 3;
            case MeshDataType::Mat4: return 4;
            default: return 1;
        }
    }
    int ScalarElementsCount() const{
        switch(type){
            case MeshDataType::Float: return 1;
            case MeshDataType::Float2: return 2;
            case MeshDataType::Float3: return 3;
            case MeshDataType::Float4: return 4;
            case MeshDataType::Int: return 1;
            case MeshDataType::Int2: return 2;
            case MeshDataType::Int3: return 3;
            case MeshDataType::Int4: return 4;
            case MeshDataType::Bool: return 1;
            case MeshDataType::Mat3: return 3*3;
            case MeshDataType::Mat4: return 4*4;
            default: return 1;
        }
    }
    int AttributeDataSize() const{
        int locations = LocationsCount();
        switch(type){
            case MeshDataType::Float: return 1 * sizeof(float);
            case MeshDataType::Float2: return 2 * sizeof(float);
            case MeshDataType::Float3: return 3 * sizeof(float);
            case MeshDataType::Float4: return 4 * sizeof(float);
            case MeshDataType::Mat3: return 3 * 3 * sizeof(float);
            case MeshDataType::Mat4: return 4 * 4 * sizeof(float);
            case MeshDataType::Int: return 1 * sizeof(int);
            case MeshDataType::Int2: return 2 * sizeof(int);
            case MeshDataType::Int3: return 3 * sizeof(int);
            case MeshDataType::Int4: return 4 * sizeof(int);
            case MeshDataType::Bool: return 1 * sizeof(bool);
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
    bool PushAttribute(const std::string &name, MeshDataType type, bool normalized, const std::vector<float> &data);
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