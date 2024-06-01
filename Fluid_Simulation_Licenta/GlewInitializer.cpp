#include "GlewInitializer.h"

bool GlewInitializer::Initialize() {
    std::cout << "Initializing GLEW..." << std::endl;

    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewErr) << std::endl;
        return false;
    }

    std::cout << "GLEW initialized successfully." << std::endl;

    // Check OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    if (version == nullptr) {
        std::cerr << "Failed to get OpenGL version after GLEW initialization" << std::endl;
        return false;
    }
    std::cout << "OpenGL version: " << version << std::endl;

    return true;
}
