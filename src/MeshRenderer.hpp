#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class MeshRenderer{
private:
    Mesh mesh;
    ShaderProgram shader;
public:
    Mesh GetMesh();
    ShaderProgram GetShader();
    void SetMesh(Mesh mesh);
    void SetShader(ShaderProgram shader);
};