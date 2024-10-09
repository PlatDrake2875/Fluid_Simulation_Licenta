#include "SceneBuilder.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>

SceneBuilder::SceneBuilder(ShaderManager3D* shaderManager)
    : shaderManager(shaderManager), boundingBox(nullptr), floorMesh(nullptr) {
    createMeshes();
}

SceneBuilder::~SceneBuilder() {
    destroyMeshes();
}

void SceneBuilder::createMeshes() {
    glm::vec3 bboxMin = shaderManager->GetBoundingBoxMin();
    glm::vec3 bboxMax = shaderManager->GetBoundingBoxMax();

    // Generate vertices and indices for the bounding box
    std::vector<GLfloat> cubeVertices = generateBoundingBoxVertices(bboxMin, bboxMax);
    std::vector<GLfloat> cubeColors = {
        0.0f, 0.0f, 0.0f,  // Black
        0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f,  
        0.0f, 0.0f, 0.0f   
    };
    GLuint cubeIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        2, 3, 7, 7, 6, 2,
        1, 2, 6, 6, 5, 1,
        0, 3, 7, 7, 4, 0
    };

    boundingBox = new Mesh(cubeVertices.data(), cubeColors.data(), cubeIndices, 8, 36, GL_LINES);

    // Generate vertices and indices for the floor
    std::vector<GLfloat> floorVertices = generateFloorVertices();
    std::vector<GLfloat> floorColors = {
        0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f
    };
    GLuint floorIndices[] = {
        0, 1, 2, 2, 3, 0
    };

    floorMesh = new Mesh(floorVertices.data(), floorColors.data(), floorIndices, 4, 6, GL_TRIANGLES);
}

void SceneBuilder::updateBoundingBox() {
    glm::vec3 bboxMin = shaderManager->GetBoundingBoxMin();
    glm::vec3 bboxMax = shaderManager->GetBoundingBoxMax();

    std::vector<GLfloat> vertices = generateBoundingBoxVertices(bboxMin, bboxMax);
    boundingBox->updateVertices(vertices.data(), 8);
}

void SceneBuilder::renderMeshes() {
    auto shader = shaderManager->GetShader();
    shader->use();

    shader->setMat4("projection", shaderManager->GetCamera()->getProjectionMatrix());
    shader->setMat4("view", shaderManager->GetCamera()->getViewMatrix());
    shader->setMat4("model", glm::mat4(1.0f));

    // Set line width for the bounding box edges
    glLineWidth(5.0f);

    boundingBox->render();
    //floorMesh->render();
}

void SceneBuilder::destroyMeshes() {
    delete boundingBox;
    delete floorMesh;
}

std::vector<GLfloat> SceneBuilder::generateBoundingBoxVertices(const glm::vec3& min, const glm::vec3& max) {
    return {
        // Front face
        min.x, min.y, min.z,
        max.x, min.y, min.z,
        max.x, max.y, min.z,
        min.x, max.y, min.z,

        // Back face
        min.x, min.y, max.z,
        max.x, min.y, max.z,
        max.x, max.y, max.z,
        min.x, max.y, max.z
    };
}


std::vector<GLfloat> SceneBuilder::generateFloorVertices() {
    return {
        -100.0f, 0.0f, -100.0f,  // Bottom-left
         100.0f, 0.0f, -100.0f,  // Bottom-right
         100.0f, 0.0f,  100.0f,  // Top-right
        -100.0f, 0.0f,  100.0f   // Top-left
    };
}
