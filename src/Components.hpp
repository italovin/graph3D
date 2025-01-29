#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "BasicComponents.hpp"
#include "Base.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Resource.hpp"

struct MeshRendererComponent{
    // Reference to active mesh resource
    Resource<Mesh> mesh;
    // Reference to active material
    Resource<Material> material;
    MeshRendererComponent(Ref<Mesh> mesh, Ref<Material> material){
        this->mesh = Resource<Mesh>(mesh);
        this->material = Resource<Material>(material);
    }
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

enum class LightType{
    Point,
    Directional,
    Spot
};

struct LightComponent{
    LightType type = LightType::Point;
    glm::vec3 color = glm::vec3(1.0f);
    float colorTemperature = 6500.0f;
    // Intensity of positive light multiplier
    float intensity = 1.0f;
    // Radial distance for point or spot light range
    float range = 5.0f;
    // Cutoff attenuation value for point and spot lights. Below cutoff the attenuation coefficient 
    // is smoothly transitioned to zero
    // This value is normally betwen 0.0 and 1.0, but anyways is clamped before passing to shader
    float cutoff = 0.0f;
    // Inner cutoff angle in degrees for spot lights. This generally is between 0° and 90°
    float spotlightInnerCutoff = 15.0f;
    // Outer cutoff angle in degrees for spot lights. This generally is between 0° and 90°
    float spotlightOuterCutoff = 17.5f;
};

#endif
