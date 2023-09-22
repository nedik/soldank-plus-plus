R"(
#version 120
varying vec2 TexCoords;
uniform sampler2D text;
uniform vec3 textColor;

void main(void) {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).r);
    gl_FragColor = vec4(textColor, 1.0) * sampled;
}
)"