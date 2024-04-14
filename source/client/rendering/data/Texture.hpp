#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <expected>

namespace Soldank::Texture
{
struct TextureData
{
    unsigned int opengl_id;
    int width;
    int height;
};

enum class LoadError
{
    TextureNotFound = 0
};

std::expected<TextureData, LoadError> Load(const char* texture_path);

void Delete(unsigned int texture_id);
} // namespace Soldank::Texture

#endif
