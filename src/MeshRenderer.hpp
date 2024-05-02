#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H
#include <memory>
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class MeshRenderer{
private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<ShaderProgram> shader;
public:
    const Mesh &GetMesh() const;
    std::shared_ptr<Mesh> GetMeshRef() const;
    const ShaderProgram &GetShader() const;
    void SetMesh(const Mesh &mesh);
    void SetShader(const ShaderProgram &shader);
};
#endif