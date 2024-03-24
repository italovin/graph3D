#include "Transform.hpp"

Transform::Transform(){
    
}

glm::vec3 Transform::Right(){
    return glm::conjugate(rotation) * glm::vec3(1, 0, 0);
}

glm::vec3 Transform::Up(){
    return glm::conjugate(rotation) * glm::vec3(0, 1, 0);
}

glm::vec3 Transform::Forward(){
    return glm::conjugate(rotation) * glm::vec3(0, 0, -1) ;
}