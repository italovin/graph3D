#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class MeshRenderer{
private:
    Mesh mesh;
    ShaderProgram shader;
public:
    Mesh GetMesh() const;
    ShaderProgram GetShader() const;
    void SetMesh(Mesh mesh);
    void SetShader(ShaderProgram shader);
};
#endif