#ifndef MATERIAL_H
#define MATERIAL_H

#include <vecmath.h>

#include <cassert>
#include <iostream>

class Material {
 public:
  Vector3f emissiveColor = 0.f;
  Vector3f diffuseColor = 1.f;
  float roughness = 1.f;
  Vector3f specularColor = 1.f;
  float transparency = 0.f;
  float refractiveIndex = 1.f;
  Vector3f refractiveColor = 1.f;
  float translucency = 0.f;
  Vector3f translucentColor = 0.f;
};

#endif  // MATERIAL_H
