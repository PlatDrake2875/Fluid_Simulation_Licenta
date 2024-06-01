#ifndef GLEW_INITIALIZER_H
#define GLEW_INITIALIZER_H

// Compatibility stuff
#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

class GlewInitializer {
public:
    GlewInitializer() = default;
    ~GlewInitializer() = default;

    bool Initialize();
};

#endif // GLEW_INITIALIZER_H
