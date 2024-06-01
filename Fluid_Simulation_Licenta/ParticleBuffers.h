#ifndef PARTICLE_BUFFERS_H
#define PARTICLE_BUFFERS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "ParticleData.h"


class ParticleBuffers {
public:
    ParticleBuffers(size_t particleCount);
    ~ParticleBuffers();

    void InitBuffers(size_t particleCount);

    void UpdateData(const std::vector<glm::vec2>& positions, const std::vector<glm::vec2>& velocities, const std::vector<glm::vec2>& predictedPositions, const std::vector<glm::vec2>& densities);
    void UpdateAllBuffers(const ParticleData& particleData);
    void RetrieveData(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities, std::vector<glm::vec2>& predictedPositions, std::vector<glm::vec2>& densities);
    GLuint GetSpatialOffsetsBuffer() const;
    GLuint GetSpatialIndicesBuffer() const;
    void DebugBufferData();
    void CheckGLError(const std::string& operation);

    void RetrieveDebugData(std::vector<glm::uint>& debugValues);

private:
    GLuint positionsBuffer;
    GLuint predictedPositionsBuffer;
    GLuint velocitiesBuffer;
    GLuint densitiesBuffer;
    GLuint spatialIndicesBuffer;
    GLuint spatialOffsetsBuffer;
    GLuint debugBuffer;

    size_t particleCount;
};

#endif // PARTICLE_BUFFERS_H
