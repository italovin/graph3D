#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "BasicComponents.hpp"
#include "Base.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

struct MeshRendererComponent{
    // Reference to active mesh resource
    Ref<Mesh> mesh;
    // Reference to active shader program
    Ref<Material> material;
    MeshRendererComponent(const Ref<Mesh> &mesh, const Ref<Material> &material) :
    mesh(mesh), material(material){}
};

struct CameraComponent{
    // It's true when is the main camera in the scene (current viewpoint)
    bool isMain = true;
    // When true, the camera is perspective; otherwise, is orthographic
    bool isPerspective = true;
    float orthographicSize = 3.0f;
    // Adjusts aspect automatically with window dimensions
    bool autoAspect = true;
    float aspectRatio = 1.7777f;
    float fieldOfView = 45.0f;
    float farPlane = 100.0f;
    float nearPlane = 0.1f;
};

#endif
