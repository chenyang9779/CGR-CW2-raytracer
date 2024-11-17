#ifndef TONE_MAPPING_H
#define TONE_MAPPING_H

// Include the Vector3 class, which likely represents a 3D vector with x, y, z components
#include "../camera/vector3.h" // Assuming Vector3 is defined in the camera folder

// Function declaration for tone mapping
// Applies tone mapping to the given color vector based on exposure and optional color bounds
// Parameters:
// - color: The input HDR color as a Vector3 (representing R, G, B values)
// - exposure: The exposure level to adjust the brightness of the image
// - minColor: The minimum RGB values for tone mapping (used to normalize the color range)
// - maxColor: The maximum RGB values for tone mapping (used to normalize the color range)
// Returns: A tone-mapped color as a Vector3
Vector3 toneMap(const Vector3 &color, float exposure, const Vector3 &minColor, const Vector3 &maxColor);

#endif // TONE_MAPPING_H
