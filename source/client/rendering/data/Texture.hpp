#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

namespace Soldat::Texture
{
unsigned int Load(const char* texture_path, int* texture_width, int* texture_height);
unsigned int Load(const char* texture_path);

void Delete(unsigned int texture_id);
} // namespace Soldat::Texture

#endif
