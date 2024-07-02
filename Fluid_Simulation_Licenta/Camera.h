#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float aspect, float nearPlane, float farPlane);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getModelMatrix() const;

    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; updateCameraVectors(); }

    glm::vec3 getFront() const { return front; }
    void setFront(const glm::vec3& frnt) { front = frnt; updateCameraVectors(); }

    void processKeyboard(int direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

    void setDebugMode(bool debug) { debugMode = debug; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    bool debugMode = false;  // Flag for debugging mode

    void updateCameraVectors();
    void printDebugInfo() const;  // Method to print debug info
};

#endif // CAMERA_H
