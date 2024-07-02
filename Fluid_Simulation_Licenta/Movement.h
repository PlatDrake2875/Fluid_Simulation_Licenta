#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Camera.h"
#include <unordered_set>

class Movement {
public:
    Movement(Camera& camera);
    void processInput(float deltaTime);
    void keyPress(unsigned char key);
    void keyRelease(unsigned char key);
    void mousePress(int button, int state, int xpos, int ypos);
    void processMouseMovement(int xpos, int ypos);
    void processMouseScroll(int button, int dir, int x, int y);

private:
    Camera& camera;
    bool firstMouse;
    float lastX;
    float lastY;
    bool mouseButtonPressed;
    std::unordered_set<unsigned char> keys; // Set to store active keys
};

#endif // MOVEMENT_H
