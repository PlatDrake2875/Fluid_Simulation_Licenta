// FluidSimulationKernels.glsl
// Scaling factors as uniform variables
uniform float Poly6ScalingFactor;
uniform float SpikyPow3ScalingFactor;
uniform float SpikyPow2ScalingFactor;
uniform float SpikyPow3DerivativeScalingFactor;
uniform float SpikyPow2DerivativeScalingFactor;

float SmoothingKernelPoly6(float dst, float radius) {
    if (dst < radius) {
        float v = radius * radius - dst * dst;
        return v * v * v * Poly6ScalingFactor;
    }
    return 0.0;
}

float SpikyKernelPow3(float dst, float radius) {
    if (dst < radius) {
        float v = radius - dst;
        return v * v * v * SpikyPow3ScalingFactor;
    }
    return 0.0;
}

float SpikyKernelPow2(float dst, float radius) {
    if (dst < radius) {
        float v = radius - dst;
        return v * v * SpikyPow2ScalingFactor;
    }
    return 0.0;
}

float DerivativeSpikyPow3(float dst, float radius) {
    if (dst <= radius) {
        float v = radius - dst;
        return -v * v * SpikyPow3DerivativeScalingFactor;
    }
    return 0.0;
}

float DerivativeSpikyPow2(float dst, float radius) {
    if (dst <= radius) {
        float v = radius - dst;
        return -v * SpikyPow2DerivativeScalingFactor;
    }
    return 0.0;
}