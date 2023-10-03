#include "Texture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <cstddef>
#include <iostream>
#include <span>

namespace Soldat::Texture
{
TextureData Load(const char* texture_path)
{
    unsigned int texture_id = 0;
    int texture_width = 0;
    int texture_height = 0;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int texture_nr_channels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(
      texture_path, &texture_width, &texture_height, &texture_nr_channels, STBI_rgb_alpha);
    if (data != nullptr) {
        std::span pixels{ data, static_cast<size_t>(texture_height * texture_width * 4) };

        // Changing fully green pixels to be transparent
        for (int y = 0; y < texture_height; y++) {
            for (int x = 0; x < texture_width; x++) {
                if (pixels[(x + y * texture_width) * 4 + 0] == 0 &&
                    pixels[(x + y * texture_width) * 4 + 1] == 255 &&
                    pixels[(x + y * texture_width) * 4 + 2] == 0) {
                    pixels[(x + y * texture_width) * 4 + 3] = 0;
                }
            }
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     texture_width,
                     texture_height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     pixels.data());
    } else {
        std::cerr << "Error: Failed to load texture " << texture_path << std::endl;
    }
    stbi_image_free(data);

    return { .opengl_id = texture_id, .width = texture_width, .height = texture_height };
}

void Delete(unsigned int texture_id)
{
    glDeleteTextures(1, &texture_id);
}
} // namespace Soldat::Texture
