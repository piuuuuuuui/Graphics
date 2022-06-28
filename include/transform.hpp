#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "object3d.hpp"

class Transform : public Object3D {
 public:
  Transform() = delete;

  Transform(const Affine3d &transform, Object3D *obj) : o(obj) {
    tr = transform;
    inv = transform.inverse();
    bbox = o->getBBox().transformed(tr);
  }

  ~Transform() {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) {
    Vector3d trSource = inv * r.origin;
    Vector3d trDirection = inv.linear() * r.direction;
    if (1. / DBL_EPSILON < trSource.squaredNorm() + trDirection.squaredNorm())
      return false;
    if (!o->intersect(Ray(trSource, trDirection), h, obj, tmin)) return false;
    h.point = tr * h.point;
    h.normal = (inv.linear().transpose() * h.normal).normalized();
    return true;
  }

 protected:
  Object3D *o;  // un-transformed object
  Affine3d tr, inv;
};

#endif  // TRANSFORM_H
