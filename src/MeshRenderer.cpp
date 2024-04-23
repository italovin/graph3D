#include "MeshRenderer.hpp"

Mesh MeshRenderer::GetMesh() const{
    return mesh;
}

ShaderProgram MeshRenderer::GetShader() const{
    return shader;
}

void MeshRenderer::SetMesh(Mesh mesh){
    this->mesh = mesh;
}

void MeshRenderer::SetShader(ShaderProgram shader){
    this->shader = shader;
}