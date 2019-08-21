#version 400

#define NUM_VERTICES 4

layout(vertices = NUM_VERTICES) out;
uniform vec3 eye;

float get_tess_level(float z)
{
    return max(1, ((z + 100) * 18) / 50 + 12);
}

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float zavg = 0;
    for (int i = 0; i < NUM_VERTICES; i++) {
        zavg += gl_in[i].gl_Position.z;
    }
    zavg /= NUM_VERTICES;
    zavg -= eye.z;

    gl_TessLevelOuter[0] = get_tess_level(zavg);
    gl_TessLevelOuter[1] = get_tess_level(gl_in[0].gl_Position.z);
    gl_TessLevelOuter[2] = get_tess_level(zavg);
    gl_TessLevelOuter[3] = get_tess_level(gl_in[2].gl_Position.z);
    gl_TessLevelInner[0] = get_tess_level(zavg);
    gl_TessLevelInner[1] = get_tess_level(zavg);
}