#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H
#include <memory>
#include "Base.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

class MeshRenderer{
private:
    Ref<Mesh> mesh;
    Ref<Shader> shader;
public:
    const Ref<Mesh> &GetMesh() const;
    const Ref<Shader> &GetShader() const;
    void SetMesh(const Ref<Mesh> &mesh);
    void SetShader(const Ref<Shader> &shader);
};
#endif