#version 400

#define CDR 3.14159265 / 180.0

layout(vertices = 16) out;
uniform float d;
uniform float t;

void main()
{
    vec4 P = gl_in[gl_InvocationID].gl_Position;
    float theta = 30 * CDR;
    float v = 1;
    vec4 initial_d = P;
    float g = 9.81;

    float vy = v * sin(theta);
    float vh = v * cos(theta);

    P.y = P.y + vy * t - (g*t*t)/2;
    P.x = P.x + initial_d.x*vh*t;
    P.z = P.z + initial_d.z*vh*t;

    gl_out[gl_InvocationID].gl_Position = P;

    int dmin = 10;
    int dmax = 200;
    int Llow = 2;
    int Lhigh = 40;

    float L = max(1, ((d - dmin) / (dmax - dmin)) * (Llow - Lhigh) + Lhigh);

    gl_TessLevelOuter[0] = gl_TessLevelOuter[1] = gl_TessLevelOuter[2] = gl_TessLevelOuter[3] =
    gl_TessLevelInner[0] = gl_TessLevelInner[1] = L;
}
