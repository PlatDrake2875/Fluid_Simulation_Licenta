#ifndef GLUT_INITIALIZER_H
#define GLUT_INITIALIZER_H

#include <GL/freeglut.h>
#include <iostream>

class GlutInitializer {
public:
    GlutInitializer(int argc, char** argv);
    ~GlutInitializer() = default;

    bool Initialize();
    void InitializeWindow(const char* title, int width, int height, int xPos, int yPos);

private:
    int argc;
    char** argv;
};

#endif // GLUT_INITIALIZER_H
