#include "Transform.hpp"

Transform::Transform(){
    
}

glm::vec3 Transform::Right(){
    return (rotation) * glm::vec3(1, 0, 0);
}

glm::vec3 Transform::Up(){
    return (rotation) * glm::vec3(0, 1, 0);
}

glm::vec3 Transform::Forward(){
    return (rotation) * glm::vec3(0, 0, 1);
}