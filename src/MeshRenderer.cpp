#include "MeshRenderer.hpp"

const Ref<Mesh> &MeshRenderer::GetMesh() const{
    return mesh;
}

const Ref<ShaderProgram> &MeshRenderer::GetShader() const{
    return shader;
}

void MeshRenderer::SetMesh(const Ref<Mesh> &mesh){
    this->mesh = mesh;
}

void MeshRenderer::SetShader(const Ref<ShaderProgram> &shader){
    this->shader = shader;
}