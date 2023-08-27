#pragma once

#include <string>

enum PMSPolygonType : unsigned char;
struct PMSColor;

namespace Utils
{
PMSColor GetPolygonColorByType(PMSPolygonType polygonType);
PMSColor GetColorByPolygonType(PMSPolygonType polygonType);
int GetPolygonPriorityByType(PMSPolygonType polygon_type);

/**
 * \brief Checks if there is an intersection between segments AB and CD.
 * http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
 */
bool SegmentsIntersect(float aX,
                       float aY,
                       float bX,
                       float bY,
                       float cX,
                       float cY,
                       float dX,
                       float dY);

// http://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
float Sign(float aX, float aY, float bX, float bY, float cX, float cY);
} // namespace Utils
