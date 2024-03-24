#include "Transform.hpp"
#include "glm/gtc/quaternion.hpp"

class Camera{
private:
    float sensitivity = 0.5f;
public:
    Transform transform;
    Camera();
    Camera(const glm::vec3 &position);
    Camera(const glm::vec3 &position, const glm::vec3 &eulerAngles);
    void ProcessMouseMovement(float xOffset, float yOffset);
};
