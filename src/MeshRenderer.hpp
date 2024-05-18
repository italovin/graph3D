#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H
#include <memory>
#include "Base.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class MeshRenderer{
private:
    Ref<Mesh> mesh;
    Ref<ShaderProgram> shader;
public:
    const Ref<Mesh> &GetMesh() const;
    const Ref<ShaderProgram> &GetShader() const;
    void SetMesh(const Ref<Mesh> &mesh);
    void SetShader(const Ref<ShaderProgram> &shader);
};
#endif