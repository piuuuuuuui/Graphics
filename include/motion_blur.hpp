#ifndef MOTION_BLUR_H
#define MOTION_BLUR_H

#include "transform.hpp"

class MotionBlur : public Transform {
 public:
  MotionBlur() = delete;

  MotionBlur(const Affine3d &transform, Object3D *obj)
      : Transform(transform, obj) {
    Eigen::MatrixPower<Matrix4d> tr_pow(tr.matrix()), inv_pow(inv.matrix());
    tr_pows.resize(NUM_ITERS);
    inv_pows.resize(NUM_ITERS);
    for (int i = 0; i < NUM_ITERS; ++i) {
      tr_pows[i].matrix() = tr_pow(i / FPS);
      inv_pows[i].matrix() = inv_pow(i / FPS);
      bbox.extend(o->getBBox().transformed(tr_pows[i]));
    }
  }

  ~MotionBlur() {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) {
    if (__glibc_unlikely(frame != iter)) {
      frame = iter;
      tr = tr_pows.at(frame);
      inv = inv_pows.at(frame);
    }
    return Transform::intersect(r, h, obj, tmin);
  }

 protected:
  int frame = -1;
  vector<Affine3d> tr_pows, inv_pows;
};

#endif  // MOTION_BLUR_H
