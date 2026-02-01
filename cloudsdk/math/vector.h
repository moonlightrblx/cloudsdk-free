#pragma once
#include <cmath>

namespace math {
    struct vector2 {
        float x, y;

        vector2() : x(0.f), y(0.f) {}
        vector2(float _x, float _y) : x(_x), y(_y) {}

        float distance(vector2 v) {
            return sqrtf(powf(v.x - x, 2) + powf(v.y - y, 2));
        }
    };

    struct vector3 {
        float x, y, z;

        vector3() : x(0.f), y(0.f), z(0.f) {}
        vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        float dot(vector3 v) {
            return x * v.x + y * v.y + z * v.z;
        }

        float distance(vector3 v) {
            return sqrtf(powf(v.x - x, 2) + powf(v.y - y, 2) + powf(v.z - z, 2));
        }
    };
}
