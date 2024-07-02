#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

uniform float maxSpeed;

void main() {
    FragColor = vec4(vertexColor * maxSpeed, 1.0);
}