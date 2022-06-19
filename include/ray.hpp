#ifndef RAY_H
#define RAY_H

#include "utils.hpp"

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {
 public:
  Ray() = delete;

  Ray(const Vector3f &o, const Vector3f &d) : origin(o), direction(d) {}

  Vector3f pointAtParameter(float t) const { return origin + direction * t; }

  Vector3f origin;
  Vector3f direction;
  float refractiveIndex = 1.f;
  float translucency = 0.f;
  Vector3f translucentColor = Vector3f::Ones();
  Vector3f attenuation = Vector3f::Ones();
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
  os << "Ray <" << r.origin << ", " << r.direction << ">";
  return os;
}

#endif  // RAY_H
