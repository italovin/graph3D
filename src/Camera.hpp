#include "Transform.hpp"

class Camera{
public:
    Transform transform;
    glm::vec3 front;
    glm::vec3 up;
    Camera();
};
