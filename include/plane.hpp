#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"

class Plane : public Object3D {
 public:
  Plane() = delete;

  Plane(const Vector3d &normal, double d, Material *m) : Object3D(m) {
    double norm = normal.norm();
    this->n = normal / norm;
    this->d = d / norm;
    bbox.min() = Vector3d::Constant(-DBL_LARGE);
    bbox.max() = Vector3d::Constant(DBL_LARGE);
  }

  ~Plane() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override {
    double cos = -n.dot(r.direction);
    double t = (n.dot(r.origin) - d) / cos;
    if (!(tmin < t && t < h.t)) return false;
    h.t = t;
    h.point = r.pointAtParameter(t);
    h.normal = n;
    h.material = mtl;
    obj = this;
    return true;
  }

 protected:
  Vector3d n;
  double d;
};

#endif  // PLANE_H
