#ifndef MESH_H
#define MESH_H
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>

enum MeshTopology{
    Triangle,
    Lines,
    LineStrip
};

enum MeshDataType{
    MeshFloat32,
    MeshInt32
};

struct MeshAttribute{
    int index;
    int size;
    MeshDataType type;
    bool normalized;
    bool operator == (const MeshAttribute &attribute){
        return (attribute.index == index) && (attribute.size == size) && (attribute.type == type) && 
        (attribute.normalized == normalized);
    }
    unsigned int DataSize(){
        switch(type){
            case MeshFloat32:
                return size*sizeof(float);
                break;
            case MeshInt32:
                return size*sizeof(int);
                break;
        }
        return 0;
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

    int Stride(){
        int stride = 0;
        for(auto &&attribute : attributes){
            stride += attribute.DataSize();
        }
        return stride;
    }
};

struct MeshAttributeData{
    std::string name;
    std::vector<float> data;
    unsigned int dataSize;
    MeshAttribute attribute;
};

class Mesh{
private:
    std::vector<unsigned int> indices;
    MeshTopology topology;
    std::vector<MeshAttributeData> attributesData;
    MeshLayout layout;
public:
    ~Mesh();
    void SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology);
    void PushAttribute(const std::string &name, int index, const std::vector<float> &data, int size, bool normalized);
    int GetAttributesCount();
    std::vector<int> GetAttributeDatasSizes();
    int GetMeshDataSize();
    int GetIndicesCount();
    int GetIndicesSize();
    MeshLayout GetLayout();
    MeshTopology GetTopology();
    const std::vector<MeshAttributeData> &GetAttributesDatas();
    const std::vector<unsigned int> &GetIndices();
};
#endif