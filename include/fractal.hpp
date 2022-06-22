#ifndef FRACTAL_H
#define FRACTAL_H

#include <vector>

#include "transform.hpp"

class Fractal : public Object3D {
 public:
  Fractal() = delete;

  Fractal(const vector<Affine3f> &transforms, Object3D *obj) : o(obj) {
    for (auto tr : transforms) fragments.push_back(Transform(tr, this));
  }

  ~Fractal() {}

  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj,
                         float tmin) override {
    bool result = o->intersect(r, h, obj, tmin);
    for (auto fragment : fragments)
      result |= fragment.intersect(r, h, obj, tmin);
    return result;
  }

 protected:
  Object3D *o;  // un-transformed object
  vector<Transform> fragments;
};

#endif  // FRACTAL_H