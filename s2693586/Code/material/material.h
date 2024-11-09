#ifndef MATERIAL_H
#define MATERIAL_H

#include "../camera/vector3.h"  // Assuming you have a Vector3 class defined in "vector3.h"

class Material {
public:
    Vector3 diffuseColor;
    Vector3 specularColor;
    float kd;  // Diffuse coefficient
    float ks;  // Specular coefficient
    float specularExponent;
    bool isReflective;
    float reflectivity;
    bool isRefractive;
    float refractiveIndex;

    Material(const Vector3& diffuse = Vector3(0.8f, 0.8f, 0.8f),
             const Vector3& specular = Vector3(1.0f, 1.0f, 1.0f),
             float kd = 0.9f,
             float ks = 0.1f,
             float specularExponent = 20.0f,
             bool isReflective = false,
             float reflectivity = 0.0f, 
             bool isRefractive = false,
             float refractiveIndex = 0.0f)
        : diffuseColor(diffuse),
          specularColor(specular),
          kd(kd),
          ks(ks),
          specularExponent(specularExponent),
          isReflective(isReflective),
          reflectivity(reflectivity), 
          isRefractive(isRefractive),
          refractiveIndex(refractiveIndex)  {}
};
#endif