R"(
#version 120
uniform mat4 transform;
attribute vec3 position;
attribute vec4 color;

varying vec4 vertexColor;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
    vertexColor = color;
}
)"