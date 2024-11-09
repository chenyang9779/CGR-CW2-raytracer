#include "tone_mapping.h"

Vector3 toneMap(const Vector3 &color, float exposure)
{
    // Scale the color by an exposure factor
    Vector3 mappedColor = color / exposure;

    // Clamp the mapped color between 0 and 1 to ensure valid RGB values
    // return mappedColor.clamp(0.0f, 1.0f);
    return color;
}
