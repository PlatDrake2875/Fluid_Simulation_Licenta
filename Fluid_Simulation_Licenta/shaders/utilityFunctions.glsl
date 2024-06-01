// Utility Functions
float DensityKernel(float dst, float radius) {
    return SpikyKernelPow2(dst, radius);
}

float NearDensityKernel(float dst, float radius) {
    return SpikyKernelPow3(dst, radius);
}

float DensityDerivative(float dst, float radius) {
    return DerivativeSpikyPow2(dst, radius);
}

float NearDensityDerivative(float dst, float radius) {
    return DerivativeSpikyPow3(dst, radius);
}

float ViscosityKernel(float dst, float radius) {
    return SmoothingKernelPoly6(dst, smoothingRadius);
}

float PressureFromDensity(float density)
{
	return (density - targetDensity) * pressureMultiplier;
}

float NearPressureFromDensity(float nearDensity)
{
	return nearPressureMultiplier * nearDensity;
}
