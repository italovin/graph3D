#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class MeshRenderer{
private:
    Mesh mesh;
    ShaderProgram shader;
public:
    const Mesh &GetMesh() const;
    const ShaderProgram &GetShader() const;
    void SetMesh(const Mesh &mesh);
    void SetShader(const ShaderProgram &shader);
};
#endif