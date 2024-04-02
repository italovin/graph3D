#include "Camera.hpp"

Camera::Camera(){
}

Camera::Camera(const glm::vec3 &position){
    transform.position = position;
}

Camera::Camera(const glm::vec3 &position, const glm::vec3 &eulerAngles){
    transform.position = position;
    transform.eulerAngles(eulerAngles);
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset){
    float factor = 0.01f;
    xOffset *= factor*sensitivity;
    yOffset *= factor*sensitivity;
    glm::quat qPitch = glm::angleAxis(yOffset, glm::vec3(1, 0, 0));
    glm::quat qYaw = glm::angleAxis(-xOffset, glm::vec3(0, 1, 0));
    transform.rotation =  qPitch * transform.rotation * qYaw;
    transform.rotation  = glm::normalize(transform.rotation);
}