#ifndef TONE_MAPPING_H
#define TONE_MAPPING_H

#include "../camera/vector3.h"  // Assuming Vector3 is defined in the camera folder

// Vector3 toneMap(const Vector3 &color, float exposure);

Vector3 toneMap(const Vector3 &color, float exposure, const Vector3 &minColor, const Vector3 &maxColor);
#endif // TONE_MAPPING_H
