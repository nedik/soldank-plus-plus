#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <optional>
#include <array>

namespace Soldank::Renderer
{
unsigned int CreateVBO(const std::vector<float>& vertices, GLenum usage);
void ModifyVBOVertices(unsigned int vbo, const std::vector<float>& vertices, int offset = 0);
unsigned int CreateEBO(const std::vector<unsigned int>& indices, GLenum usage);

void FreeVBO(unsigned int vbo);
void FreeEBO(unsigned int ebo);

void SetupVertexArray(unsigned int vbo,
                      std::optional<unsigned int> ebo,
                      bool has_color = true,
                      bool has_texture = true);

void BindTexture(unsigned int texture);

void DrawArrays(GLenum mode, GLint first, GLsizei count);
void DrawElements(GLenum mode,
                  GLsizei count,
                  GLenum type,
                  std::optional<unsigned int> indices_offset);
} // namespace Soldank::Renderer

#endif
