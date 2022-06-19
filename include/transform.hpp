#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vector>

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

class MotionBlur : public Transform {
 public:
  MotionBlur() = delete;

  MotionBlur(const Transform &transform) : Transform(transform) {
    MatrixPower<Matrix4f> tr_pow(tr.matrix()), inv_pow(inv.matrix());
    tr_pows.resize(NUM_ITERS);
    inv_pows.resize(NUM_ITERS);
    for (int i = 0; i < NUM_ITERS; i++) {
      tr_pows[i] = tr_pow(i / FPS);
      inv_pows[i] = inv_pow(i / FPS);
    }
  }

  ~MotionBlur() {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
    if (frame != r.frame) {
      frame = r.frame;
      tr.matrix() = tr_pows.at(frame);
      inv.matrix() = inv_pows.at(frame);
    }
    return Transform::intersect(r, h, obj, tmin);
  }

 protected:
  int frame = 0;
  vector<Matrix4f> tr_pows, inv_pows;
};

#endif  // TRANSFORM_H
