#version 400

layout(vertices = 4) out;

void main()
{
    gl_out[gl_InvocationID].gl_Position
      = gl_in[gl_InvocationID].gl_Position;
    gl_TessLevelOuter[0] =
    gl_TessLevelOuter[1] =
    gl_TessLevelOuter[2] =
    gl_TessLevelOuter[3] =
    gl_TessLevelInner[0] =
    gl_TessLevelInner[1] = 6;
}