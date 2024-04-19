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
    std::vector<unsigned int> indices;
    MeshTopology topology;
    
    template <typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    struct MeshAttribute{
        std::string name;
        std::vector<T> data;
        int size;
        bool normalized;
    };
public:
    void SetMeshSize(unsigned int size);
    void SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology);
    void PushMeshData(const std::vector<float> data, int attributeIndex);
    void PushAttribute(const std::string &name, MeshDataType dataType, int count);
};