#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 color[3];
out vec4 oColor;

vec4 calculateN(vec3 p1, vec3 p2, vec3 p3) {
    return normalize(vec4(cross(p2 - p1, p3 - p1), 0));
}


void main()
{
    vec4 material = vec4(0, 1, 0, 1);
    float greyLevel = 0.2;

    vec4 ambient = material * greyLevel;

    vec4 n = calculateN(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);
    vec4 l = normalize(vec4(-1, -1, 0, 0)); //todo use a light source
    vec4 diffuse = dot(l, n) * material;

    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        oColor = ambient + diffuse;
        EmitVertex();
    }
    EndPrimitive();
}