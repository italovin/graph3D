#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform{
private:
    glm::vec3 _eulerAngles = glm::vec3(0, 0, 0);
public:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    const glm::vec3 &eulerAngles() {
        _eulerAngles = glm::degrees(glm::eulerAngles(rotation));
        return this->_eulerAngles;
    }
    void eulerAngles(const glm::vec3 &eulerAngles) {
        glm::quat quatX = glm::angleAxis(glm::radians(eulerAngles).x, glm::vec3(1, 0, 0));
        glm::quat quatY = glm::angleAxis(glm::radians(eulerAngles).y, glm::vec3(0, 1, 0));
        glm::quat quatZ = glm::angleAxis(glm::radians(eulerAngles).z, glm::vec3(0, 0, 1));
        rotation = glm::normalize(quatX * quatY * quatZ);
        this->_eulerAngles = eulerAngles;
    }
    glm::vec3 Right();
    glm::vec3 Up();
    glm::vec3 Forward();
};
#endif