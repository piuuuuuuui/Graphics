#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"

class Plane : public Object3D {
 public:
  Plane() = delete;

  Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    float norm = normal.norm();
    this->n = normal / norm;
    this->d = d / norm;
  }

  ~Plane() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    float cos = -n.dot(r.direction);
    float t = (n.dot(r.origin) - d) / cos;
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