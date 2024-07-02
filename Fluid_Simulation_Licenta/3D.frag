#version 450

in vec3 fragColor;
out vec4 color;

uniform float maxSpeed;

void main()
{
    color = vec4(fragColor * maxSpeed, 1.0); 
}
