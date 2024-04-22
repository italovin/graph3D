#include "MeshRenderer.hpp"

Mesh MeshRenderer::GetMesh(){
    return mesh;
}

ShaderProgram MeshRenderer::GetShader(){
    return shader;
}

void MeshRenderer::SetMesh(Mesh mesh){
    this->mesh = mesh;
}

void MeshRenderer::SetShader(ShaderProgram shader){
    this->shader = shader;
}