#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.hpp"

class Material {
 public:
  Vector3f emissiveColor = Vector3f::Zero();
  Vector3f diffuseColor = Vector3f::Ones();
  float roughness = 1.f;
  Vector3f specularColor = Vector3f::Ones();
  float transparency = 0.f;
  float refractiveIndex = 1.f;
  Vector3f refractiveColor = Vector3f::Ones();
  float translucency = 0.f;
  Vector3f translucentColor = Vector3f::Zero();
};

#endif  // MATERIAL_H
