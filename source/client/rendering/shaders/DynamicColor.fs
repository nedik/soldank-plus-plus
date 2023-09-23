R"(
#version 120
varying vec2 vertexTexturePosition;

uniform sampler2D textureData;
uniform vec4 color;

void main()
{
    gl_FragColor = texture2D(textureData, vertexTexturePosition) * color;
}
)"