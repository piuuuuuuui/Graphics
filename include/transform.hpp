#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "object3d.hpp"

class Transform : public Object3D {
 public:
  Transform() = delete;

  Transform(const Affine3f &transform, Object3D *obj) : o(obj) {
    tr = transform;
    inv = transform.inverse();
  }

  ~Transform() {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
    Vector3f trSource = inv * r.origin;
    Vector3f trDirection = inv.linear() * r.direction;
    if (!o->intersect(Ray(trSource, trDirection), h, obj, tmin)) return false;
    h.point = tr * h.point;
    h.normal = (inv.linear().transpose() * h.normal).normalized();
    return true;
  }

 protected:
  Object3D *o;  // un-transformed object
  Affine3f tr, inv;
};

#endif  // TRANSFORM_H
