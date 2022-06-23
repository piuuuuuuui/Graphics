#ifndef PORTAL_H
#define PORTAL_H

#include "transform.hpp"

class Portal : public Transform {
 public:
  Portal() = delete;

  Portal(const Affine3f &transform, Object3D *obj) : Transform(transform, obj) {
    bbox.extend(obj->getBBox());
  }

  ~Portal() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    bool result = false;
    if (o->intersect(r, h, obj, tmin)) {
      obj = this;
      result = true;
      m = tr;
    }
    if (Transform::intersect(r, h, obj, tmin)) {
      obj = this;
      result = true;
      m = inv;
    }
    return result;
  }

  bool getNextRay(const Hit &hit, Ray &ray) override {
    ray.origin = m * hit.point;
    ray.direction = m.linear() * ray.direction;
    return true;
  }

 private:
  Affine3f m;
};

#endif  // PORTAL_H
