#ifndef PLANE_H
#define PLANE_H

#include <vecmath.h>

#include <cmath>

#include "object3d.hpp"

class Plane : public Object3D {
 public:
  Plane() = delete;

  Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    float norm = normal.length();
    this->n = normal / norm;
    this->d = d / norm;
  }

  ~Plane() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    float cos = -Vector3f::dot(n, r.direction);
    float t = (Vector3f::dot(n, r.origin) - d) / cos;
    if (t < h.t && t > tmin) {
      h.t = t;
      h.point = r.pointAtParameter(t);
      h.normal = n;
      h.material = material;
      obj = this;
      return true;
    }
    return false;
  }

 protected:
  Vector3f n;
  float d;
};

#endif  // PLANE_H
