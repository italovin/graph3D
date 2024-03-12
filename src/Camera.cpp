#include "Camera.hpp"

Camera::Camera(){
    CalculateCameraVectors();
}

Camera::Camera(const glm::vec3 position){
    transform.position = position;
    CalculateCameraVectors();
}

Camera::Camera(const glm::vec3 position, const glm::vec3 rotation){
    transform.position = position;
    transform.rotation = rotation;
    CalculateCameraVectors();
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset){
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    transform.rotation.y  += xOffset;
    transform.rotation.x += yOffset;
    transform.rotation.x = glm::clamp(transform.rotation.x, -89.0f, 89.0f);
    CalculateCameraVectors();
}

void Camera::CalculateCameraVectors(){
    glm::vec3 front;
    front.x = cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
    front.y = sin(glm::radians(transform.rotation.x));
    front.z = sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
    this->front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, glm::vec3(0, 1.0f, 0)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up    = glm::normalize(glm::cross(right, front));
}