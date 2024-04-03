#include "Calc.hpp"

#include <cmath>

namespace Soldank::Calc
{
float Distance(const glm::vec2& p1, const glm::vec2& p2)
{
    return glm::length(p2 - p1);
}

float Vec2Length(const glm::vec2& v)
{
    return glm::length(v);
}

glm::vec2 Vec2Normalize(const glm::vec2& v)
{
    auto magnitude = glm::length(v);
    if (magnitude < 0.001) {
        return { 0.0F, 0.0F };
    }

    return v / magnitude;
}

float Vec2Angle(const glm::vec2& v)
{
    return (float)(-atan2(0.0, 1.0) - atan2(v.y, v.x));
}

float PointLineDistance(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
{
    auto u = ((p3.x - p1.x) * (p2.x - p1.x) + (p3.y - p1.y) * (p2.y - p1.y)) /
             (std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));

    auto x = p1.x + u * (p2.x - p1.x);
    auto y = p1.y + u * (p2.y - p1.y);

    return (float)std::sqrt(std::pow(x - p3.x, 2) + std::pow(y - p3.y, 2));
}

glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t)
{
    return glm::mix(a, b, t);
}

float Lerp(float a, float b, float t)
{
    return std::lerp(a, b, t);
}
} // namespace Soldank::Calc
