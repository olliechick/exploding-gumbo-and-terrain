#version 330

in vec4 oColor;
in vec3 textureWeights;
in vec2 textureCoords;

uniform sampler2D grass;
uniform sampler2D water;
uniform sampler2D snow;

void main()
{
    vec4 grassColor = texture(grass, textureCoords);
    vec4 waterColor = texture(water, textureCoords);
    vec4 snowColor = texture(snow, textureCoords);

    gl_FragColor = oColor *
                  (grassColor * textureWeights.y
                 + waterColor * textureWeights.x
                 + snowColor  * textureWeights.z);
}
