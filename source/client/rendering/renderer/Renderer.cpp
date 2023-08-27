#include "Renderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Soldat::Renderer
{
unsigned int CreateVBO(const std::vector<float>& vertices, GLenum usage)
{
    unsigned int vbo = 0;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (long long)vertices.size() * (long long)sizeof(float),
                 vertices.data(),
                 usage);

    return vbo;
}

unsigned int CreateEBO(const std::vector<unsigned int>& indices, GLenum usage)
{
    unsigned int ebo = 0;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (long long)indices.size() * (long long)sizeof(unsigned int),
                 indices.data(),
                 usage);

    return ebo;
}

void FreeVBO(unsigned int vbo)
{
    glDeleteBuffers(1, &vbo);
}

void FreeEBO(unsigned int ebo)
{
    glDeleteBuffers(1, &ebo);
}

void SetupVertexArray(unsigned int vbo, std::optional<unsigned int> ebo, bool has_texture)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if (ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    }

    GLsizei stride = 0;
    if (has_texture) {
        stride = 9 * sizeof(float);
    } else {
        stride = 7 * sizeof(float);
    }

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    // color attribute
    // NOLINTNEXTLINE(performance-no-int-to-ptr,cppcoreguidelines-pro-type-cstyle-cast)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    if (has_texture) {
        // texture coord attribute
        // NOLINTNEXTLINE(performance-no-int-to-ptr,cppcoreguidelines-pro-type-cstyle-cast)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
}

void BindTexture(unsigned int texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
}

void DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
}

void DrawElements(GLenum mode,
                  GLsizei count,
                  GLenum type,
                  std::optional<unsigned int> indices_offset)
{
    if (indices_offset) {
        unsigned int offset = *indices_offset;
        // NOLINTNEXTLINE(performance-no-int-to-ptr,cppcoreguidelines-pro-type-cstyle-cast)
        glDrawElements(mode, count, type, (const GLvoid*)(uintptr_t)(offset));
    } else {
        glDrawElements(mode, count, type, nullptr);
    }
}
} // namespace Soldat::Renderer