R"(
#version 120
uniform mat4 transform;
attribute vec3 position;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
}
)"