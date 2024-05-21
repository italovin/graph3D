#include "MeshRenderer.hpp"

const Ref<Mesh> &MeshRenderer::GetMesh() const{
    return mesh;
}

const Ref<Shader> &MeshRenderer::GetShader() const{
    return shader;
}

void MeshRenderer::SetMesh(const Ref<Mesh> &mesh){
    this->mesh = mesh;
}

void MeshRenderer::SetShader(const Ref<Shader> &shader){
    this->shader = shader;
}