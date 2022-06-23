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
    if (t < h.t && t > tmin) {
      Vector3f p = r.pointAtParameter(t);
      if ((p - o).norm() <= radius) {
        h.t = t;
        h.point = p;
        h.normal = n;
        h.material = material;
        obj = this;
        return true;
      }
    }
    return false;
  }

 protected:
  Vector3f o, n;
  float radius;
};

#endif  // DISK_H
