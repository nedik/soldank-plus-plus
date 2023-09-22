R"(
#version 120
varying vec4 vertexColor;
varying vec2 vertexTexturePosition;

uniform sampler2D textureData;

void main()
{
    gl_FragColor = texture2D(textureData, vertexTexturePosition) * vertexColor;
}
)"