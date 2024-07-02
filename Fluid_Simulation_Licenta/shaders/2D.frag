#version 450 core

in vec2 fragVelocity; 
out vec4 FragColor;

uniform float maxSpeed;

void main() {
    float speed = length(fragVelocity);

    float normalizedSpeed = clamp(speed / maxSpeed, 0.0, 1.0);

    vec3 color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), normalizedSpeed);

    FragColor = vec4(color, 1.0);
}
