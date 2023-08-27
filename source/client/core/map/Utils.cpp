#include "Utils.hpp"

#include "PMSEnums.hpp"
#include "PMSStructs.hpp"

namespace Utils
{
PMSColor GetPolygonColorByType(PMSPolygonType polygonType)
{
    switch (polygonType) {
        case ptNORMAL:
            return PMSColor(112, 128, 144, 255);

        case ptONLY_BULLETS_COLLIDE:
            return PMSColor(122, 204, 41, 255);

        case ptONLY_PLAYERS_COLLIDE:
            return PMSColor(204, 204, 41, 255);

        case ptNO_COLLIDE:
            return PMSColor(41, 204, 41, 255);

        case ptICE:
            return PMSColor(41, 204, 204, 255);

        case ptDEADLY:
            return PMSColor(204, 41, 122, 255);

        case ptBLOODY_DEADLY:
            return PMSColor(204, 41, 204, 255);

        case ptHURTS:
            return PMSColor(204, 41, 41, 255);

        case ptREGENERATES:
            return PMSColor(41, 41, 204, 255);

        case ptLAVA:
            return PMSColor(204, 122, 41, 255);
            // TODO: handle other polygon types.

        case ptBOUNCY:
            return PMSColor(41, 122, 204, 255);

        case ptEXPLOSIVE:
            return { 204, 204, 204, 255 };

        default:
            return PMSColor(0, 0, 0, 255);
    }
}

PMSColor GetColorByPolygonType(PMSPolygonType polygonType)
{
    PMSColor color = GetPolygonColorByType(polygonType);
    return color;
}

int GetPolygonPriorityByType(PMSPolygonType polygon_type)
{
    switch (polygon_type) {
        case ptNORMAL:
            return 2;

        case ptONLY_PLAYERS_COLLIDE:
            return 2;

        case ptNO_COLLIDE:
            return 0;

        case ptICE:
            return 2;

        case ptDEADLY:
            return 4;

        case ptBLOODY_DEADLY:
            return 4;

        case ptHURTS:
            return 3;

        case ptEXPLOSIVE:
            return 4;

        default:
            return 1;
    }
}

bool SegmentsIntersect(float aX,
                       float aY,
                       float bX,
                       float bY,
                       float cX,
                       float cY,
                       float dX,
                       float dY)
{
    float segmentABCenterX = bX - aX;
    float segmentABCenterY = bY - aY;
    float segmentCDCenterX = dX - cX;
    float segmentCDCenterY = dY - cY;

    float s, t;
    s = (-segmentABCenterY * (aX - cX) + segmentABCenterX * (aY - cY)) /
        (-segmentCDCenterX * segmentABCenterY + segmentABCenterX * segmentCDCenterY);
    t = (segmentCDCenterX * (aY - cY) - segmentCDCenterY * (aX - cX)) /
        (-segmentCDCenterX * segmentABCenterY + segmentABCenterX * segmentCDCenterY);

    return s >= 0 && s <= 1 && t >= 0 && t <= 1;
}

float Sign(float aX, float aY, float bX, float bY, float cX, float cY)
{
    return (aX - cX) * (bY - cY) - (bX - cX) * (aY - cY);
}
} // namespace Utils
