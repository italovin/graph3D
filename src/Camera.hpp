#ifndef CAMERA_H
#define CAMERA_H
#include "Transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"

class Camera{
private:
    float sensitivity = 0.5f;
public:
    Transform transform;
    Camera();
    Camera(const glm::vec3 &position);
    Camera(const glm::vec3 &position, const glm::vec3 &eulerAngles);
    glm::mat4 GetViewMatrix() const;
};
#endif
