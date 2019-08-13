#version 400

#define CDR 3.14159265 / 180.0
#define NUM_VERTICES 16

layout(vertices = NUM_VERTICES) out;
uniform float d;
uniform float t;

void main()
{
    // Calculate position based on explosion equations

    vec4 P = vec4(0, 0, 0, 1);

    float v = 1;// speed
    float g = 9.81;// gravity

    // Find middle of patch
    vec4 initialP = vec4(0);
    for (int i = 0; i < NUM_VERTICES; i++) {
        initialP += gl_in[i].gl_Position;
    }
    initialP /= NUM_VERTICES;

    // Offset of point from middle of patch
    vec4 offset = gl_in[gl_InvocationID].gl_Position - initialP;

    float theta = acos(initialP.y / length(initialP));
    float vy = v * sin(theta) * 30;
    float vh = v * cos(theta);

    float h = gl_in[gl_InvocationID].gl_Position.y;
    // from j.mp/31zrnoH    t = [V₀* sin(α)    +     √(    V₀* sin(α))²       + 2*g*h)] / g
    float time_to_reach_floor = (vy + sqrt(pow(vy, 2) + 2*g*h)) / g;
    float adjusted_t = min(t, time_to_reach_floor);// make t not go above time_to_reach_floor

    P.y = initialP.y + vy * adjusted_t - (g*adjusted_t*adjusted_t)/2.0;
    P.xz = initialP.xz + initialP.xz * vh * adjusted_t;

    if (P.y < 0) P.y = 0;// don't go below floor
    P += offset;// add the offset

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
