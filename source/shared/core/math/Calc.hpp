#ifndef __CALC_HPP__
#define __CALC_HPP__

#include "core/math/Glm.hpp"

#include <cmath>
#include <algorithm>

namespace Soldat::Calc
{
float Distance(const glm::vec2& p1, const glm::vec2& p2);
float Vec2Length(const glm::vec2& v);
glm::vec2 Vec2Normalize(const glm::vec2& v);
float Vec2Angle(const glm::vec2& v);
float PointLineDistance(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);

glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t);
float Lerp(float a, float b, float t);
} // namespace Soldat::Calc

#endif
