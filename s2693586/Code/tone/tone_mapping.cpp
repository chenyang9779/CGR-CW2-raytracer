#include "tone_mapping.h"
#include <vector>


// Vector3 toneMap(const Vector3 &color, float exposure)
// {
//     // Scale the color by an exposure factor
//     Vector3 mappedColor = color * exposure;

//     // Clamp the mapped color between 0 and 1 to ensure valid RGB values
//     // return mappedColor.clamp(0.0f, 1.0f);
//     return color;
// }
// float computeAverageLuminance(const std::vector<Vector3> &colors) {
//     float sumLogLuminance = 0.0f;

//     for (const auto &color : colors) {
//         float luminance = color.luminance();
//         sumLogLuminance += std::log(1e-6f + luminance);  // Log luminance to avoid bias and prevent log(0)
//     }

//     return std::exp(sumLogLuminance / colors.size());  // Geometric mean
// }



// Vector3 toneMap(const Vector3 &color, float averageLuminance, float exposure)
// {
//     // Scale the luminance by a "key" value, typically between 0.18 and 0.36
//     float scaledLuminance = exposure / averageLuminance;

//     // Apply exposure adjustment to the color
//     Vector3 hdrColor = color * scaledLuminance;

//     // Apply Reinhard tone mapping to compress the color values
//     Vector3 mappedColor = hdrColor / (hdrColor + Vector3(1.0f));

//     // Optional: Apply gamma correction (gamma = 2.2)
//     float gamma = 2.2f;
//     mappedColor = Vector3(
//         std::pow(mappedColor.x, 1.0f / gamma),
//         std::pow(mappedColor.y, 1.0f / gamma),
//         std::pow(mappedColor.z, 1.0f / gamma)
//     );

//     // Clamp final values between 0 and 1 to ensure valid RGB output
//     mappedColor = mappedColor.clamp(0.0f, 1.0f);

//     return mappedColor;
// }

Vector3 toneMap(const Vector3 &color, float exposure, const Vector3 &minColor, const Vector3 &maxColor, const Vector3 &backgroundColour)
{
    Vector3 normalizedColor;
    // Normalize color between the global minimum and maximum
    if (color == backgroundColour) {
        normalizedColor = color;
    } else{
        normalizedColor = (color - minColor) / (maxColor - minColor);
    }


    // Apply exposure
    Vector3 mappedColor = normalizedColor; // * exposure;

    // ACES tone mapping
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    mappedColor = (mappedColor*(a*mappedColor+b))/(mappedColor*(c*mappedColor+d)+e);

    // Gamma correction (optional, for more perceptual accuracy)
    // constexpr float gamma = 1.2f;
    // mappedColor.x = pow(mappedColor.x, 1.0f / gamma);
    // mappedColor.y = pow(mappedColor.y, 1.0f / gamma);
    // mappedColor.z = pow(mappedColor.z, 1.0f / gamma);

    // Scale to [0, 255] range and clamp
    return mappedColor.clamp(0.0f, 1.0f);
}


// Vector3 toneMap(const Vector3 &color, float exposure, const Vector3 &minColor, const Vector3 &maxColor)
// {
//     // Normalize color between the global minimum and maximum (avoiding divide-by-zero)
//     Vector3 range = maxColor - minColor;
//     Vector3 normalizedColor(
//         (color.x - minColor.x) / (range.x + 1e-5f),
//         (color.y - minColor.y) / (range.y + 1e-5f),
//         (color.z - minColor.z) / (range.z + 1e-5f)
//     );

//     // Apply exposure and scale to [0, 255] range
//     Vector3 mappedColor = normalizedColor * exposure;

//     // Clamp the mapped color between 0 and 255 to ensure valid RGB values
//     return mappedColor.clamp(0.0f, 1.0f);
// }


