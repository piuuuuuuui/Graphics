#ifndef MOTION_BLUR_H
#define MOTION_BLUR_H

#include <vecmath.h>

#include "transform.hpp"
#include "utils.hpp"

class MotionBlur : public Object3D {
 public:
  MotionBlur() = delete;

  MotionBlur(Object3D *obj) : o(obj) {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
    return Transform(getTransform(), o).intersect(r, h, obj, tmin);
  }

  virtual Matrix4f getTransform() { return Matrix4f::identity(); }

 protected:
  Object3D *o;  // un-transformed object
};

class TranslationBlur : public MotionBlur {
 public:
  TranslationBlur() = delete;

  TranslationBlur(const Vector3f &velocity, Object3D *obj)
      : MotionBlur(obj), v(velocity) {}

  ~TranslationBlur() {}

  virtual Matrix4f getTransform() override {
    return Matrix4f::translation(v * (RAND_U - 0.5f));
  }

 protected:
  Object3D *o;  // un-transformed object
  Vector3f v;
};

class RotationBlur : public MotionBlur {
 public:
  RotationBlur() = delete;

  RotationBlur(const Vector3f &angular_velocity, Object3D *obj)
      : MotionBlur(obj) {
    omega = angular_velocity.length();
    if (0 < omega) n = angular_velocity / omega;
  }

  ~RotationBlur() {}

  virtual Matrix4f getTransform() override {
    return Matrix4f::rotation(n, omega * (RAND_U - 0.5f));
  }

 protected:
  Object3D *o;  // un-transformed object
  Vector3f n;
  float omega;
};

class ScalationBlur : public MotionBlur {
 public:
  ScalationBlur() = delete;

  ScalationBlur(const Vector3f &scalation, Object3D *obj)
      : MotionBlur(obj), s(scalation) {}

  ~ScalationBlur() {}

  virtual Matrix4f getTransform() override {
    return Matrix4f::scaling(1.f + s * (RAND_U - 0.5f));
  }

 protected:
  Object3D *o;  // un-transformed object
  Vector3f s;
};

#endif  // MOTION_BLUR_H
