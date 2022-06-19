#ifndef HIT_H
#define HIT_H

#include <vecmath.h>

#include <random>

#include "ray.hpp"

class Material;

struct Hit {
  float t = __FLT_MAX__;
  float u = 0.f, v = 0.f;
  Vector3f point = 0.f, normal = 0.f;
  Material *material = nullptr;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
  os << "Hit <" << h.t << ", " << h.normal << ">";
  return os;
}

#endif  // HIT_H
