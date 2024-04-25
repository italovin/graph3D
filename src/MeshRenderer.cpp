#include "MeshRenderer.hpp"

const Mesh &MeshRenderer::GetMesh() const{
    return mesh;
}

const ShaderProgram &MeshRenderer::GetShader() const{
    return shader;
}

void MeshRenderer::SetMesh(const Mesh &mesh){
    this->mesh = mesh;
}

void MeshRenderer::SetShader(const ShaderProgram &shader){
    this->shader = shader;
}