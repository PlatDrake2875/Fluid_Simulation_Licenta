#include "Movement.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut_std.h>

Movement::Movement(Camera& camera)
    : camera(camera), firstMouse(true), lastX(400.0f), lastY(300.0f), mouseButtonPressed(false) {}

void Movement::processInput(float deltaTime)
{
    if (keys.find('w') != keys.end())
        camera.processKeyboard(0, deltaTime); // FORWARD
    if (keys.find('s') != keys.end())
        camera.processKeyboard(1, deltaTime); // BACKWARD
    if (keys.find('a') != keys.end())
        camera.processKeyboard(2, deltaTime); // LEFT
    if (keys.find('d') != keys.end())
        camera.processKeyboard(3, deltaTime); // RIGHT
    if (keys.find(' ') != keys.end())
        camera.processKeyboard(4, deltaTime); // UP (SPACE)
    if (keys.find(8) != keys.end()) // ASCII code for BACKSPACE
        camera.processKeyboard(5, deltaTime); // DOWN (BACKSPACE)
}

void Movement::keyPress(unsigned char key)
{
    keys.insert(key);
    std::cerr << "Key pressed: " << key << std::endl;
}

void Movement::keyRelease(unsigned char key)
{
    keys.erase(key);
    std::cerr << "Key released: " << key << std::endl;
}

void Movement::mousePress(int button, int state, int xpos, int ypos)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseButtonPressed = true;
        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }
        std::cerr << "Mouse button pressed at: (" << xpos << ", " << ypos << ")\n";
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        mouseButtonPressed = false;
        firstMouse = true; // Reset firstMouse flag when the button is released
        std::cerr << "Mouse button released at: (" << xpos << ", " << ypos << ")\n";
    }
}

void Movement::processMouseMovement(int xpos, int ypos)
{
    if (mouseButtonPressed) {
        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos) - lastX;
        float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        camera.processMouseMovement(xoffset, yoffset);
        std::cerr << "Mouse moved: xoffset: " << xoffset << ", yoffset: " << yoffset << std::endl;
    }
}

void Movement::processMouseScroll(int button, int dir, int x, int y)
{
    if (dir > 0)
        camera.processMouseScroll(-1.0f); // zoom in
    else
        camera.processMouseScroll(1.0f); // zoom out
    std::cerr << "Mouse scroll: direction: " << dir << std::endl;
}
