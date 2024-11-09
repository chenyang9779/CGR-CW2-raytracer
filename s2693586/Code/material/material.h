#ifndef MATERIAL_H
#define MATERIAL_H

#include "../camera/vector3.h"

class Material
{
public:
    Vector3 diffuseColor;
    Vector3 specularColor;
    float kd; // Diffuse coefficient
    float ks; // Specular coefficient
    float specularExponent;
    bool isReflective; // Is the material reflective
    float reflectivity; // Reflectivity coefficient

    Material()
        : diffuseColor(0.8, 0.8, 0.8), specularColor(1.0, 1.0, 1.0),
          kd(0.8), ks(0.5), specularExponent(20.0), isReflective(false), reflectivity(0.5) {}

    Material(const Vector3 &diffuseColor, const Vector3 &specularColor, float kd, float ks, float specularExponent, bool isReflective, float reflectivity)
        : diffuseColor(diffuseColor), specularColor(specularColor), kd(kd), ks(ks),
          specularExponent(specularExponent), isReflective(isReflective), reflectivity(reflectivity) {}
};

#endif // MATERIAL_H
