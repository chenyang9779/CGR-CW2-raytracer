#ifndef MATERIAL_H
#define MATERIAL_H

#include "../camera/vector3.h" // Include the Vector3 class for representing RGB colors

// Class representing the material properties of a geometric object
class Material
{
public:
  // Color properties
  Vector3 diffuseColor;  // Diffuse color of the material (used for diffuse shading)
  Vector3 specularColor; // Specular color of the material (used for highlights)

  // Coefficients for shading
  float kd;               // Diffuse coefficient: controls the strength of diffuse shading
  float ks;               // Specular coefficient: controls the strength of specular highlights
  float specularExponent; // Exponent for specular highlights (controls sharpness)

  // Reflection properties
  bool isReflective;  // Indicates whether the material is reflective
  float reflectivity; // Reflectivity value (0.0 = no reflection, 1.0 = full reflection)

  // Refraction properties
  bool isRefractive;     // Indicates whether the material is refractive
  float refractiveIndex; // Index of refraction for the material (e.g., 1.5 for glass)

  // Constructor to initialize material properties with default values
  Material(const Vector3 &diffuse = Vector3(0.8f, 0.8f, 0.8f),  // Default diffuse color (gray)
           const Vector3 &specular = Vector3(1.0f, 1.0f, 1.0f), // Default specular color (white)
           float kd = 0.9f,                                     // Default diffuse coefficient
           float ks = 0.1f,                                     // Default specular coefficient
           float specularExponent = 20.0f,                      // Default specular sharpness
           bool isReflective = false,                           // Default: not reflective
           float reflectivity = 0.0f,                           // Default reflectivity
           bool isRefractive = false,                           // Default: not refractive
           float refractiveIndex = 0.0f)                        // Default refractive index
      : diffuseColor(diffuse),
        specularColor(specular),
        kd(kd),
        ks(ks),
        specularExponent(specularExponent),
        isReflective(isReflective),
        reflectivity(reflectivity),
        isRefractive(isRefractive),
        refractiveIndex(refractiveIndex)
  {
  }
};

#endif // MATERIAL_H
