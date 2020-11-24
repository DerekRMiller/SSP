#pragma once 

#include "Percussa.h"



#include <algorithm>


extern const char* percussaParamsName[Percussa::sspLast];

extern const char* percussaParamsText[Percussa::sspLast];

inline float constrainFloat(float v, float vMin, float vMax) {
    return std::max<float>(vMin, std::min<float>(vMax, v));
}

static float cv2Pitch(float r) {
    static constexpr float p1 = 0.0f; // first C note
    static constexpr float p2 = 0.2f; // second C note
    static constexpr float scale = 12.0f / (p2 - p1);
    float arg = r;
    arg = arg - p1;
    arg = arg * scale;
    return arg;
}


float pitch2Cv(float r) {
    static constexpr float p1 = 0.0f; // first C note
    static constexpr float p2 = 0.2f; // second C note
    static constexpr float iscale = (p2 - p1) / 12.0f;
    float arg = r;
    arg = arg * iscale;
    arg = arg + p1;
    return arg;
}

