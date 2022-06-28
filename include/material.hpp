#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.hpp"

class Material {
 public:
  Vector3d emissiveColor = Vector3d::Zero();
  Vector3d diffuseColor = Vector3d::Ones();
  double roughness = 1.;
  Vector3d specularColor = Vector3d::Ones();
  double opacity = 1.;
  double refractiveIndex = 1.;
  Vector3d refractiveColor = Vector3d::Ones();
  Matrix4d translucency = Matrix4d::Zero();
  Texture *emissiveMap = nullptr;
  Texture *diffuseMap = nullptr;
  Texture *specularMap = nullptr;
  Texture *opacityMap = nullptr;
  Texture *bumpMap = nullptr;
};

#endif  // MATERIAL_H
