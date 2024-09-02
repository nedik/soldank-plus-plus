R"(
#version 120
uniform mat4 transform;
attribute vec3 position;

varying vec3 vertexPosition;

void main()
{
    vertexPosition = position;
    gl_Position = transform * vec4(position, 1.0);
}
)"