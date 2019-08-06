#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;
out vec4 oColor;

void main()
{
    for (int i = 0; i < gl_in.length(); i++) {
        gl_Position = mvpMatrix * gl_in[i].gl_Position;
        oColor = vec4(0, 0, 1, 1);
        //todo oColor = ambOut + diffOut + specOut;
        EmitVertex();
    }
    EndPrimitive();
}