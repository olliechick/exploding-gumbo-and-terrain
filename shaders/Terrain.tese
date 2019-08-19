#version 400

layout(quads, equal_spacing, ccw) in;

uniform sampler2D heightMap;
vec4 posn;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    posn = (1-u)*(1-v) * gl_in[0].gl_Position
         + u * (1-v) *   gl_in[1].gl_Position
         + u * v *       gl_in[2].gl_Position
         + (1-u) * v *   gl_in[3].gl_Position;

    float x1 = -45, x2 = 45, z1 = -100, z2 = 0;

    float s = (posn.x - x1)/(x2 - x1);
    float t = (posn.z - z1)/(z2 - z1);

    vec4 color = texture(heightMap, vec2(s, t));
    posn.y = color.r * 10;

    gl_Position = posn;
}
