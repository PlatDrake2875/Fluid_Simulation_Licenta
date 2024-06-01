#ifndef PARTICLE_DATA_H
#define PARTICLE_DATA_H

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct ParticleData {
    std::vector<glm::vec2> positions;
    std::vector<glm::vec2> velocities;
    std::vector<glm::vec2> predictedPositions;
    std::vector<glm::vec2> densities;
    std::vector<glm::uvec3> spatialIndices;
    std::vector<GLuint> spatialOffsets;
};

#endif // PARTICLE_DATA_H
