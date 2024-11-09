#ifndef TONE_MAPPING_H
#define TONE_MAPPING_H

#include "../camera/vector3.h"  // Assuming Vector3 is defined in the camera folder

Vector3 toneMap(const Vector3 &color, float exposure);

#endif // TONE_MAPPING_H
