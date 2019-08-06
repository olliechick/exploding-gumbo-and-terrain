#version 400

layout(quads, equal_spacing, ccw) in;
uniform mat4 mvpMatrix;
vec4 posn;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    posn = (1-u)*(1-u) * ((1-v)*(1-v)*gl_in[6].gl_Position
                          + 2*v*(1-v)*gl_in[7].gl_Position
                          + v*v*gl_in[8].gl_Position)
         + 2*(1-u)*u   * ((1-v)*(1-v)*gl_in[3].gl_Position
                          + 2*v*(1-v)*gl_in[4].gl_Position
                          + v*v*gl_in[5].gl_Position)
         + u*u         * ((1-v)*(1-v)*gl_in[0].gl_Position
                          + 2*v*(1-v)*gl_in[1].gl_Position
                          + v*v*gl_in[2].gl_Position);
    
    gl_Position = mvpMatrix * posn;
}
