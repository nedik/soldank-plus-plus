R"(
#version 120
varying vec3 vertexPosition;

uniform float outerRadius;
uniform float innerRadius;
uniform vec4 color;

void main()
{
    float dist = sqrt(dot(vertexPosition.xy, vertexPosition.xy));
    if (dist > outerRadius || dist < innerRadius) {
        discard;
    }
    gl_FragColor = color;
}
)"