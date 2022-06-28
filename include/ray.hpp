#ifndef RAY_H
#define RAY_H

#include "utils.hpp"

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {
 public:
  Ray() = delete;

  Ray(const Vector3d &o, const Vector3d &d) : origin(o), direction(d) {}

  Vector3d pointAtParameter(double t) const { return origin + direction * t; }

  Vector3d origin;
  Vector3d direction;

  double refractiveIndex = 1.;
  Matrix4d translucency = Matrix4d::Zero();
  Affine3d colorTrans = Affine3d::Identity();
};

inline ostream &operator<<(ostream &os, const Ray &r) {
  os << "Ray <" << r.origin << ", " << r.direction << ">";
  return os;
}

#endif  // RAY_H
