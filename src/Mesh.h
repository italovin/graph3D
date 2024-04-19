#include <vector>
#include <string>
#include <glm/glm.hpp>

enum MeshTopology{
    Triangle,
    Line,
    LineStrip
};

enum MeshDataType{
    MeshFloat32,
    MeshFloat16,
    MeshInt32
};

class Mesh{
private:
    void *data;
    std::vector<unsigned int> indices;
    MeshTopology topology;
    struct MeshAttribute{
        std::string name;
        MeshDataType dataType;
        int count;
    };
    
    std::vector<MeshAttribute> attributes;
public:
    void SetMeshSize(unsigned int size);
    void SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology);
    void PushAttribute(const std::string &name, MeshDataType dataType, int count);
};