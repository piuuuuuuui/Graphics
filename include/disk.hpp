#ifndef DISK_H
#define DISK_H

#include "object3d.hpp"

class Disk : public Object3D {
 public:
  Disk() = delete;

  Disk(const Vector3f &center, const Vector3f &normal, float radius,
       Material *m)
      : Object3D(m), o(center), n(normal), radius(radius) {}

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    float cos = -n.dot(r.direction);
    float t = (n.dot(r.origin - o)) / cos;
    if (t <= tmin || h.t <= t) return false;
    Vector3f p = r.pointAtParameter(t);
    if (radius <= (p - o).norm()) return false;
    h.t = t;
    h.point = p;
    h.normal = n;
    h.material = material;
    obj = this;
    return true;
  }

 protected:
  Vector3f o, n;
  float radius;
};

#endif  // DISK_H
