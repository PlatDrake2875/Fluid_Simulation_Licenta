#ifndef GPUSORT_H
#define GPUSORT_H

#include "ComputeShader.h"
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

class GPUSort {
public:
    GPUSort();
    ~GPUSort();

    void SetBuffers(const std::vector<glm::uvec3>& spatialIndices, const std::vector<glm::uint>& spatialOffsets);
    void UpdateBuffers(const std::vector<glm::uvec3>& spatialIndices, const std::vector<glm::uint>& spatialOffsets);
    void RetrieveSpatialData(std::vector<glm::uvec3>& spatialIndices, std::vector<glm::uint>& spatialOffsets);
    void Sort();
    void SortAndCalculateOffsets();

private:
    ComputeShader* sortComputeShader;

    GLuint indexBuffer;
    GLuint offsetBuffer;

    int NextPowerOfTwo(int value);

    void CheckGLError(const std::string& operation);
};

#endif // GPUSORT_H
