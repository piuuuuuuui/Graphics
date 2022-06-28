#ifndef HIT_H
#define HIT_H

#include "ray.hpp"

class Material;

struct Hit {
  double t = DBL_MAX;
  double u = 0., v = 0.;
  Vector3d point = Vector3d::Zero(), normal = Vector3d::Zero();
  Material *material = nullptr;
};

inline ostream &operator<<(ostream &os, const Hit &h) {
  os << "Hit <" << h.t << ", " << h.normal << ">";
  return os;
}

#endif  // HIT_H
