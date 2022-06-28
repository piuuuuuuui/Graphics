#ifndef DISK_H
#define DISK_H

#include "object3d.hpp"

class Disk : public Object3D {
 public:
  Disk() = delete;

  Disk(const Vector3d &center, const Vector3d &normal, double radius,
       Material *m)
      : Object3D(m), o(center), n(normal), radius(radius) {}

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override {
    double cos = -n.dot(r.direction);
    double t = (n.dot(r.origin - o)) / cos;
    if (!(tmin < t && t < h.t)) return false;
    Vector3d p = r.pointAtParameter(t);
    if (!((p - o).norm() < radius)) return false;
    h.t = t;
    h.point = p;
    h.normal = n;
    h.material = mtl;
    obj = this;
    return true;
  }

 protected:
  Vector3d o, n;
  double radius;
};

#endif  // DISK_H
