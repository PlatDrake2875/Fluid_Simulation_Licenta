#version 450 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inVelocity;

out vec2 fragVelocity;

uniform mat4 projection;

void main() {
    fragVelocity = inVelocity;
    gl_Position = projection * vec4(inPosition, 0.0, 1.0);
}
