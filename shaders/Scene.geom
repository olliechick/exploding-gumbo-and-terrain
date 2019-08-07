#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;
uniform vec4 material;
out vec4 oColor;

vec3 calculateNormal(float x1, float y1, float z1,
float x2, float y2, float z2,
float x3, float y3, float z3)
{
    float nx, ny, nz;
    nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
    ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
    nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
    return vec3(nx, ny, nz);
}

vec3 calculateNormal(vec4 v1, vec4 v2, vec4 v3)
{
    return calculateNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
}

void main()
{
    vec4 ambOut, diffOut, specOut = vec4(0);
    vec4 white = vec4(1.0);

    ambOut = 0.1 * material;

    vec4 position = gl_in[0].gl_Position;
    vec3 normal = vec3(normalize(calculateNormal(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position)));
    vec4 posnEye = mvMatrix * position;
    vec4 normalEye = norMatrix * vec4(normal, 0);
    vec4 lgtVec = normalize(lightPos - posnEye);
    float diffTerm = max(dot(lgtVec, normalEye), 0.0);
    diffOut = diffTerm * material;

    vec4 viewVec = normalize(vec4(-posnEye.xyz, 0));
    vec4 halfVec = normalize(lgtVec + viewVec);
    float shininess = 100.0;
    float specTerm = max(dot(halfVec, normalEye), 0);
    specOut = white * pow(specTerm, shininess);


    for (int i = 0; i < gl_in.length(); i++) {
        gl_Position = mvpMatrix * gl_in[i].gl_Position;
        oColor = ambOut + diffOut + specOut;
        EmitVertex();
    }
    EndPrimitive();
}