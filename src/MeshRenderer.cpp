#include "MeshRenderer.hpp"

const Mesh &MeshRenderer::GetMesh() const{
    return *mesh;
}

std::shared_ptr<Mesh> MeshRenderer::GetMeshRef() const{
    return mesh;
}

const ShaderProgram &MeshRenderer::GetShader() const{
    return *shader;
}

void MeshRenderer::SetMesh(const Mesh &mesh){
    this->mesh = std::make_shared<Mesh>(mesh);
}

void MeshRenderer::SetShader(const ShaderProgram &shader){
    this->shader = std::make_shared<ShaderProgram>(shader);
}