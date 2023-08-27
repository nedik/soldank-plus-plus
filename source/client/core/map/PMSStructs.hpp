#pragma once

#include "PMSConstants.hpp"
#include "PMSEnums.hpp"
#include "Utils.hpp"
#include <vector>
#include <cmath>

struct DOSTime {
    unsigned short second : 5, minute : 6, hour : 5;
};

struct DOSDate {
    unsigned short day : 5, month : 4, year : 7;
};

struct PMSCollider {
    int active;
    float x, y, radius;
};

struct PMSColor {
    unsigned char blue, green, red, alpha;

    PMSColor() { red = 255, green = 255, blue = 255, alpha = 255; }

    PMSColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        red = r, green = g, blue = b, alpha = a;
    }
};

struct PMSVector {
    float x, y, z;
};

struct PMSVertex {
    float x, y, z;
    // Those are most likely texture-related
    float rhw;
    PMSColor color;
    float textureS, textureT; // S corresponds to X axis, T corresponds to Y axis.
};

struct PMSPolygon {
    PMSVertex vertices[3];
    PMSVector perpendiculars[3];
    PMSPolygonType polygonType;
    float bounciness;

    /**
     * \brief Checks if the vertices are arranged in clock-wise order.
     * http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
     */
    bool AreVerticesClockwise()
    {
        float sum = 0.0f;

        for (unsigned int i = 0; i < 3; ++i) {
            unsigned int j = i + 1;
            if (j > 2) {
                j = 0;
            }

            sum += (vertices[j].x - vertices[i].x) * (vertices[j].y + vertices[i].y);
        }

        return sum < 0.0f;
    }

    /**
     * \brief Checks if the point at given coordinates is inside the polygon.
     */
    bool Contains(float x, float y)
    {
        bool b1, b2, b3;

        b1 = Utils::Sign(x, y, vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y) < 0.0f;

        b2 = Utils::Sign(x, y, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y) < 0.0f;

        b3 = Utils::Sign(x, y, vertices[2].x, vertices[2].y, vertices[0].x, vertices[0].y) < 0.0f;

        return (b1 == b2 && b2 == b3);
    }
};

struct PMSScenery {
    bool active;
    unsigned short style;
    int width, height;
    float x, y, rotation, scaleX, scaleY;
    int alpha;
    PMSColor color;
    int level;

    /**
     * \brief Checks if the point at given coordinates is inside the scenery.
     */
    bool Contains(float x, float y)
    {
        float rectX[4], rectY[4];

        rectX[0] = 0.0, rectY[0] = 0.0;
        rectX[1] = width * scaleX, rectY[1] = 0.0;
        rectX[2] = width * scaleX, rectY[2] = height * scaleY;
        rectX[3] = 0.0, rectY[3] = height * scaleY;

        float rotatedX, rotatedY;
        unsigned int i;
        for (i = 0; i < 4; ++i) {
            rotatedX = rectX[i] * cos(-rotation) - rectY[i] * sin(-rotation);
            rotatedY = rectX[i] * sin(-rotation) + rectY[i] * cos(-rotation);

            rectX[i] = rotatedX + this->x;
            rectY[i] = rotatedY + this->y;
        }

        bool b[4];
        for (i = 0; i < 3; ++i) {
            b[i] = Utils::Sign(x, y, rectX[i], rectY[i], rectX[i + 1], rectY[i + 1]) < 0.0f;
        }
        b[3] = Utils::Sign(x, y, rectX[3], rectY[3], rectX[0], rectY[0]) < 0.0f;

        return (b[0] == b[1] && b[1] == b[2] && b[2] == b[3]);
    }
};

struct PMSTimestamp {
    DOSTime time;
    DOSDate date;
};

struct PMSSceneryType {
    unsigned char nameLength;
    char name[SCENERY_NAME_MAX_LENGTH];
    PMSTimestamp timestamp;
};

struct PMSSector {
    unsigned short polygonsCount;
    std::vector<unsigned short> polygons;
};

struct PMSSpawnPoint {
    int active, x, y;
    PMSSpawnPointType type;
};

struct PMSWayPoint {
    bool active;
    unsigned char filler1[3];
    int id;
    int x, y;
    bool left, right, up, down, jet;
    unsigned char path;
    PMSSpecialAction specialAction;
    unsigned char c2, c3; //?
    unsigned char filler2[3];
    int connectionsCount;
    int connections[20];
};
