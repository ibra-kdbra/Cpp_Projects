#ifndef TRIGONOMETRICFUNCTIONS_TYPES_H
#define TRIGONOMETRICFUNCTIONS_TYPES_H

extern bool g_MODE_SIN;
extern bool g_MODE_COS;
extern bool g_MODE_TAN;
extern bool g_MODE_ASIN;
extern bool g_MODE_ACOS;
extern bool g_MODE_ATAN;
extern float g_offset;

enum equations {
    sine = 0,
    cosine = 1,
    tangent = 2,
    pause = 3,
    arcSine = 4,
    arcCosine = 5,
    arcTangent = 6
};

struct RGB {
    float red, green, blue;

    RGB(float red, float green, float blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }
};

#endif //TRIGONOMETRICFUNCTIONS_TYPES_H
