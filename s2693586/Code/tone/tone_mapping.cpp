#include "tone_mapping.h"
#include <vector>

Vector3 clamp(const Vector3 &value, const Vector3 &min, const Vector3 &max) {
    return Vector3(
        std::max(min.x, std::min(max.x, value.x)),
        std::max(min.y, std::min(max.y, value.y)),
        std::max(min.z, std::min(max.z, value.z))
    );
}


Vector3 toneMap(const Vector3 &color, float exposure, const Vector3 &minColor, const Vector3 &maxColor, const Vector3 &backgroundColour)
{   
    if (color == backgroundColour){
        return backgroundColour;
    }

    // Linear tone mapping
    float luminance = 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;
    float toneMappedLuminance = luminance / (luminance + 1.0f + 1e-6f);
    Vector3 mappedColor = color * (toneMappedLuminance / (luminance + 1e-6f));

    // Gamma correction (optional, for more perceptual accuracy)
    constexpr float gamma = 1.2f;
    mappedColor.x = pow(mappedColor.x, 1.0f / gamma);
    mappedColor.y = pow(mappedColor.y, 1.0f / gamma);
    mappedColor.z = pow(mappedColor.z, 1.0f / gamma);

    // Clamp the final color to the specified range
    mappedColor = clamp(mappedColor, minColor, maxColor);

    return mappedColor;
}




