#include "Transform.hpp"

class Camera{
private:
    float sensitivity = 0.5f;
    void CalculateCameraVectors();
public:
    Transform transform;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    Camera();
    Camera(const glm::vec3 position);
    Camera(const glm::vec3 position, const glm::vec3 rotation);
    void ProcessMouseMovement(float xOffset, float yOffset);
};
