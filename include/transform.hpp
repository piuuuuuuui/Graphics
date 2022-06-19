#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>

#include <cstdio>

#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
  return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D direction using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
  return (mat * Vector4f(dir, 0)).xyz();
}

class Transform : public Object3D {
 public:
  Transform() = delete;

  Transform(const Matrix4f &m, Object3D *obj) : o(obj) {
    tr = m;
    inv = m.inverse();
  }

  ~Transform() {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
    Vector3f trSource = transformPoint(inv, r.origin);
    Vector3f trDirection = transformDirection(inv, r.direction);
    if (!o->intersect(Ray(trSource, trDirection), h, obj, tmin)) return false;
    h.point = transformPoint(tr, h.point);
    h.normal = transformDirection(inv.transposed(), h.normal).normalized();
    return true;
  }

 protected:
  Object3D *o;  // un-transformed object
  Matrix4f tr, inv;
};

#endif  // TRANSFORM_H
