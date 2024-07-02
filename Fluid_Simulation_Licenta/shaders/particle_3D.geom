#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 200) out;

uniform mat4 projection;
uniform mat4 modelView;
uniform float radius;
const int num_segments = 32; 
const int num_rings = 16;

void main()
{
    vec3 center = (modelView * gl_in[0].gl_Position).xyz;
    
    for (int j = 0; j <= num_rings; ++j)
    {
        float phi = float(j) / float(num_rings) * 3.1415926;
        for (int i = 0; i <= num_segments; ++i)
        {
            float theta = float(i) / float(num_segments) * 2.0 * 3.1415926; 
            
            float x = radius * sin(phi) * cos(theta); 
            float y = radius * sin(phi) * sin(theta); 
            float z = radius * cos(phi);

            vec3 pos = center + vec3(x, y, z);
            gl_Position = projection * vec4(pos, 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
