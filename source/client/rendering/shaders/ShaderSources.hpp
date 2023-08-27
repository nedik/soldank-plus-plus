#ifndef __SHADER_SOURCES_HPP__
#define __SHADER_SOURCES_HPP__

namespace Soldat::ShaderSources
{
constexpr const char* const VERTEX_SHADER_SOURCE = R"(
        #version 120
        uniform mat4 transform;
        attribute vec3 position;
        attribute vec4 color;
        attribute vec2 texturePosition;

        varying vec4 vertexColor;
        varying vec2 vertexTexturePosition;

        void main()
        {
           gl_Position = transform * vec4(position, 1.0);
           vertexColor = color;
           vertexTexturePosition = texturePosition;
        }
    )";

constexpr const char* const FRAGMENT_SHADER_SOURCE = R"(
        #version 120
        varying vec4 vertexColor;
        varying vec2 vertexTexturePosition;

        uniform sampler2D textureData;

        void main()
        {
           gl_FragColor = texture2D(textureData, vertexTexturePosition) * vertexColor;
        }
    )";

constexpr const char* const NO_TEXTURE_VERTEX_SHADER_SOURCE = R"(
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
    )";

constexpr const char* const NO_TEXTURE_FRAGMENT_SHADER_SOURCE = R"(
        #version 120
        varying vec4 vertexColor;

        void main()
        {
           gl_FragColor = vertexColor;
        }
    )";

constexpr const char* const FONT_VERTEX_SHADER_SOURCE = R"(
        #version 120
        attribute vec4 vertex;
        varying vec2 TexCoords;
        uniform mat4 projection;

        void main(void) {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        TexCoords = vertex.zw;
        }
    )";

constexpr const char* const FONT_FRAGMENT_SHADER_SOURCE = R"(
        #version 120
        varying vec2 TexCoords;
        uniform sampler2D text;
        uniform vec3 textColor;

        void main(void) {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).r);
            gl_FragColor = vec4(textColor, 1.0) * sampled;
        }
    )";
} // namespace Soldat::ShaderSources

#endif
