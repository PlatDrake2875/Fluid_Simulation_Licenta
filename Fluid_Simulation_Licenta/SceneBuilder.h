#ifndef SCENE_BUILDER_H
#define SCENE_BUILDER_H

#include "ShaderManager3D.h"
#include "Mesh.h"
#include <vector>
#include <glm/glm.hpp>

class SceneBuilder {
public:
    SceneBuilder(ShaderManager3D* shaderManager);
    ~SceneBuilder();

    void renderMeshes();
    void updateBoundingBox();

private:
    ShaderManager3D* shaderManager;
    Mesh* boundingBox;
    Mesh* floorMesh;
    Mesh* skyboxMesh;

    void createMeshes();
    void destroyMeshes();
    std::vector<GLfloat> generateBoundingBoxVertices(const glm::vec3& min, const glm::vec3& max);
    std::vector<GLfloat> generateFloorVertices();
    std::vector<GLfloat> generateSkyboxVertices();
};

#endif // SCENE_BUILDER_H
