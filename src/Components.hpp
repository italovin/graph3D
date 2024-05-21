#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "Base.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent{
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    TransformComponent() = default;
    TransformComponent(const glm::vec3 &position) : position(position){}

    void eulerAngles(const glm::vec3 &eulerAngles) {
        glm::quat quatX = glm::angleAxis(glm::radians(eulerAngles).x, glm::vec3(1, 0, 0));
        glm::quat quatY = glm::angleAxis(glm::radians(eulerAngles).y, glm::vec3(0, 1, 0));
        glm::quat quatZ = glm::angleAxis(glm::radians(eulerAngles).z, glm::vec3(0, 0, 1));
        rotation = glm::normalize(quatX * quatY * quatZ);
    }
};

struct MeshRendererComponent{
    Ref<Mesh> mesh;
    Ref<Shader> shader;
    MeshRendererComponent(const Ref<Mesh> &mesh, const Ref<Shader> &shader) :
    mesh(mesh), shader(shader){}
};

#endif