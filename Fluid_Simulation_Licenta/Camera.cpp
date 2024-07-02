#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float aspect, float nearPlane, float farPlane)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(fov), fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane)
{
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(zoom), aspect, nearPlane, farPlane);
}

glm::mat4 Camera::getModelMatrix() const
{
    return glm::mat4(1.0f);
}

void Camera::processKeyboard(int direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == 0) // FORWARD
        position += front * velocity;
    if (direction == 1) // BACKWARD
        position -= front * velocity;
    if (direction == 2) // LEFT
        position -= right * velocity;
    if (direction == 3) // RIGHT
        position += right * velocity;
    if (direction == 4) // UP
        position += up * velocity;
    if (direction == 5) // DOWN
        position -= up * velocity;

    if (debugMode) printDebugInfo();
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();

    if (debugMode) printDebugInfo();
}

void Camera::processMouseScroll(float yoffset)
{
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= yoffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;

    if (debugMode) printDebugInfo();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void Camera::printDebugInfo() const
{
    std::cerr << "Camera Debug Info:" << std::endl;
    std::cerr << "Position: " << glm::to_string(position) << std::endl;
    std::cerr << "Front: " << glm::to_string(front) << std::endl;
    std::cerr << "Up: " << glm::to_string(up) << std::endl;
    std::cerr << "Right: " << glm::to_string(right) << std::endl;
    std::cerr << "Yaw: " << yaw << " Pitch: " << pitch << std::endl;
    std::cerr << "Zoom: " << zoom << std::endl;
    std::cerr << "--------------------------" << std::endl;
}
