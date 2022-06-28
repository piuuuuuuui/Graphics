#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.hpp"

class Material {
 public:
  Vector3d emissiveColor = Vector3d::Zero();
  Vector3d diffuseColor = Vector3d::Ones();
  double roughness = 1.;
  Vector3d specularColor = Vector3d::Ones();
  double transparency = 0.;
  double refractiveIndex = 1.;
  Vector3d refractiveColor = Vector3d::Ones();
  Matrix4d translucency = Matrix4d::Zero();
};

#endif  // MATERIAL_H
