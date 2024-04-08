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

glm::mat4 Camera::GetViewMatrix() const{
    glm::mat4 rotate = glm::mat4_cast(glm::conjugate(transform.rotation));
    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -transform.position);
    glm::mat4 view = rotate * translate;
    return view;
}