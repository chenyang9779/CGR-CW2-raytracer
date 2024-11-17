#include "tone_mapping.h"
#include <vector>

// Clamps the input color vector to ensure its components fall within the specified min and max bounds
// Parameters:
// - value: The input color vector (RGB values)
// - min: The minimum allowed RGB values
// - max: The maximum allowed RGB values
// Returns: A clamped Vector3 where each component is within the [min, max] range
Vector3 clamp(const Vector3 &value, const Vector3 &min, const Vector3 &max)
{
    return Vector3(
        std::max(min.x, std::min(max.x, value.x)), // Clamp the red channel
        std::max(min.y, std::min(max.y, value.y)), // Clamp the green channel
        std::max(min.z, std::min(max.z, value.z))  // Clamp the blue channel
    );
}

// Applies tone mapping to the input HDR color to adjust its brightness and map it to a perceptually accurate range
// Parameters:
// - color: The input HDR color (Vector3, representing RGB values)
// - exposure: The exposure value for brightness adjustment (not directly used in this function)
// - minColor: The minimum allowed color values (used for clamping the result)
// - maxColor: The maximum allowed color values (used for clamping the result)
// - backgroundColour: The background color of the scene (used to avoid unnecessary tone mapping for background pixels)
// Returns: A tone-mapped RGB color as a Vector3
Vector3 toneMap(const Vector3 &color, float exposure, const Vector3 &minColor, const Vector3 &maxColor, const Vector3 &backgroundColour)
{
    // If the color is equal to the background color, return it directly (no tone mapping needed)
    if (color == backgroundColour)
    {
        return backgroundColour;
    }

    // Calculate the luminance of the input color (perceptual brightness based on RGB weights)
    float luminance = 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;

    // Apply tone mapping to reduce luminance values to a perceptually balanced range
    float toneMappedLuminance = luminance / (luminance + 1.0f + 1e-6f);
    Vector3 mappedColor = color * (toneMappedLuminance / (luminance + 1e-6f)); // Scale color components

    // Apply gamma correction to improve perceptual accuracy (optional)
    constexpr float gamma = 1.2f;                     // Gamma value for correction
    mappedColor.x = pow(mappedColor.x, 1.0f / gamma); // Correct the red channel
    mappedColor.y = pow(mappedColor.y, 1.0f / gamma); // Correct the green channel
    mappedColor.z = pow(mappedColor.z, 1.0f / gamma); // Correct the blue channel

    // Clamp the final color values to ensure they stay within the specified min and max range
    mappedColor = clamp(mappedColor, minColor, maxColor);

    // Return the final tone-mapped and clamped color
    return mappedColor;
}
