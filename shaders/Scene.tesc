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
    vec4 centre = vec4(0);
    for (int i = 0; i < NUM_VERTICES; i++) {
        centre += gl_in[i].gl_Position;
    }
    centre /= NUM_VERTICES;

    // Offset of point from middle of patch
    vec4 offset = gl_in[gl_InvocationID].gl_Position - centre;

    float k = 20.0;
    float theta = asin(centre.y / length(centre));
    float vy = v * sin(theta) * k;
    float vh = v * cos(theta);

    float h = gl_in[gl_InvocationID].gl_Position.y;
    // from j.mp/31zrnoH    t =[V₀*sin(α)+√(V₀*sin(α))²+2 * g * h)] / g
    float time_to_reach_floor = (vy + sqrt(pow(vy, 2) + 2 * g * h)) / g;
    float adjusted_t = min(t, time_to_reach_floor);// make t not go above time_to_reach_floor

    P.y = centre.y + vy * adjusted_t - (g*adjusted_t*adjusted_t)/2.0 + offset.y;
    P.xz = centre.xz + centre.xz * vh * adjusted_t + offset.xz;

    // find the smallest (most negative) offset
    float smallest_y_offset = 0;
    for (int i = 0; i < NUM_VERTICES; i++) {
        float other_offest = gl_in[i].gl_Position.y - centre.y;
        if (other_offest < smallest_y_offset) smallest_y_offset = other_offest;
    }

    if (P.y - offset.y + smallest_y_offset < 0) {
        // some part of the fragment is below ground
        P.y = offset.y - smallest_y_offset;
    }

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
