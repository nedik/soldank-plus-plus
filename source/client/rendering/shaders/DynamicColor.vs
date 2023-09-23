R"(
#version 120
uniform mat4 transform;
attribute vec3 position;
attribute vec2 texturePosition;

varying vec2 vertexTexturePosition;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
    vertexTexturePosition = texturePosition;
}
)"