#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "BasicComponents.hpp"
#include "Base.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

struct MeshRendererComponent{
    Ref<Mesh> mesh;
    Ref<Shader> shader;
    MeshRendererComponent(const Ref<Mesh> &mesh, const Ref<Shader> &shader) :
    mesh(mesh), shader(shader){}
};

#endif