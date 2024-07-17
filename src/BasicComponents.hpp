#ifndef BASIC_COMPONENTS_H
#define BASIC_COMPONENTS_H
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
    glm::vec3 Right(){
        return (rotation) * glm::vec3(1, 0, 0);
    }

    glm::vec3 Up(){
        return (rotation) * glm::vec3(0, 1, 0);
    }

    glm::vec3 Forward(){
        return (rotation) * glm::vec3(0, 0, 1);
    }
};
#endif