#version 400

layout(vertices = 16) out;
uniform int d;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    int dmin = 10;
    int dmax = 100;
    int Llow = 2;
    int Lhigh = 20;

    int L = ((d - dmin) / (dmax - dmin)) * (Llow - Lhigh) + Lhigh;

    gl_TessLevelOuter[0] = gl_TessLevelOuter[1] = gl_TessLevelOuter[2] = gl_TessLevelOuter[3] = gl_TessLevelInner[0] = gl_TessLevelInner[1] = L;
}
