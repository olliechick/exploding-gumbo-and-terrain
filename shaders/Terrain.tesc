#version 400

#define NUM_VERTICES 4

layout(vertices = NUM_VERTICES) out;
uniform vec3 eye;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float zavg = 0;
    for (int i = 0; i < NUM_VERTICES; i++) {
        zavg += gl_in[i].gl_Position.z;
    }
    zavg /= NUM_VERTICES;
    zavg -= eye.z;

    gl_TessLevelOuter[0] =
    gl_TessLevelOuter[1] =
    gl_TessLevelOuter[2] =
    gl_TessLevelOuter[3] =
    gl_TessLevelInner[0] =
    gl_TessLevelInner[1] = max(1, ((zavg + 100) * 18) / 50 + 12);
}