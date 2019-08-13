#version 400

#define CDR 3.14159265 / 180.0
#define NUM_VERTICES 16

layout(vertices = NUM_VERTICES) out;
uniform float d;
uniform float t;

void main()
{
    // Calculate position based on explosion equations

    float v = 1; // speed
    float g = 9.81; // gravity

    // Find middle of patch
    vec4 P = vec4(0);
    for (int i = 0; i < NUM_VERTICES; i++) {
        P += gl_in[i].gl_Position;
    }
    P /= NUM_VERTICES;

    // Offset of point from middle of patch
    vec4 offset = gl_in[gl_InvocationID].gl_Position - P;

    float theta = acos(P.y / length(P));
    float vy = v * sin(theta);
    float vh = v * cos(theta);

    P.y = P.y + vy * t - (g*t*t)/2.0;

    P.xz = P.xz + P.xz * vh * t;

    if (P.y < 0) P.y = 0; // don't go below floor
    P += offset; // add the offset
    P.w = 1;

    gl_out[gl_InvocationID].gl_Position = P;

    // Calculate tessellation level based on distance from camera

    int dmin = 10;
    int dmax = 200;
    int Llow = 2;
    int Lhigh = 40;

    float L = max(1, ((d - dmin) / (dmax - dmin)) * (Llow - Lhigh) + Lhigh);

    gl_TessLevelOuter[0] = gl_TessLevelOuter[1] = gl_TessLevelOuter[2] = gl_TessLevelOuter[3] =
    gl_TessLevelInner[0] = gl_TessLevelInner[1] = L;
}
