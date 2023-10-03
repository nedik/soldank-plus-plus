#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

namespace Soldat::Texture
{
struct TextureData
{
    unsigned int opengl_id;
    int width;
    int height;
};

TextureData Load(const char* texture_path);

void Delete(unsigned int texture_id);
} // namespace Soldat::Texture

#endif
