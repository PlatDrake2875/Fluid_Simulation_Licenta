#include "GlutInitializer.h"

GlutInitializer::GlutInitializer(int argc, char** argv) : argc(argc), argv(argv) {}

bool GlutInitializer::Initialize() {
    std::cout << "Initializing GLUT..." << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

    return true;
}

void GlutInitializer::InitializeWindow(const char* title, int width, int height, int xPos, int yPos) {
    std::cout << "Creating GLUT window..." << std::endl;
    glutInitWindowSize(width, height);
    glutInitWindowPosition(xPos, yPos);
    glutCreateWindow(title);
    std::cout << "GLUT window created." << std::endl;

    // Verify that a valid OpenGL context is created
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    if (renderer && version) {
        std::cout << "Renderer: " << renderer << "\n";
        std::cout << "OpenGL version supported: " << version << "\n";
    }
    else {
        std::cerr << "Failed to create a valid OpenGL context.\n";
        std::exit(EXIT_FAILURE);
    }
}
