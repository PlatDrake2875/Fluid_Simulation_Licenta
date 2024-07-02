#ifndef PARTICLE_BUFFERS_3D_H
#define PARTICLE_BUFFERS_3D_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "ParticleData.h"
#include "ComputeShader.h"

class ParticleBuffers3D {
public:
    ParticleBuffers3D(size_t particleCount, ComputeShader* computeShader);
    ~ParticleBuffers3D();

    void InitBuffers(size_t particleCount);

    void UpdateData(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& velocities, const std::vector<glm::vec3>& predictedPositions, const std::vector<glm::vec2>& densities);
    void UpdateSpatialData(const std::vector<glm::uvec3>& spatialIndices, const std::vector<glm::uint>& spatialOffsets);
    void UpdateAllBuffers(const ParticleData3D& particleData);
    void RetrieveData(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& velocities, std::vector<glm::vec3>& predictedPositions, std::vector<glm::vec2>& densities);
    void RetrieveSpatialData(std::vector<glm::uvec3>& spatialIndices, std::vector<glm::uint>& spatialOffsets);
    GLuint GetSpatialOffsetsBuffer() const;
    GLuint GetSpatialIndicesBuffer() const;
    void DebugBufferData();
    void CheckGLError(const std::string& operation);

    void RetrieveDebugData(std::vector<glm::uint>& debugValues);

    void useComputeShader();

private:
    GLuint positionsBuffer;
    GLuint predictedPositionsBuffer;
    GLuint velocitiesBuffer;
    GLuint densitiesBuffer;
    GLuint spatialIndicesBuffer;
    GLuint spatialOffsetsBuffer;
    GLuint debugBuffer;

    size_t particleCount;

    ComputeShader* computeShader;
};

#endif // PARTICLE_BUFFERS_3D_H
