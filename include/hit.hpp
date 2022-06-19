#ifndef HIT_H
#define HIT_H

#include "ray.hpp"

class Material;

struct Hit {
  float t = __FLT_MAX__;
  float u = 0.f, v = 0.f;
  Vector3f point = Vector3f::Zero(), normal = Vector3f::Zero();
  Material *material = nullptr;
};

inline ostream &operator<<(ostream &os, const Hit &h) {
  os << "Hit <" << h.t << ", " << h.normal << ">";
  return os;
}

#endif  // HIT_H
