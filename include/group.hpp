#ifndef GROUP_H
#define GROUP_H

#include <vector>

#include "object3d.hpp"

class Group : public Object3D {
 public:
  Group() {}

  ~Group() override {}

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    if (!Object3D::intersect(r, h, obj, tmin)) return false;
    bool result = false;
    for (Object3D *object : objects)
      result |= object->intersect(r, h, obj, tmin);
    return result;
  }

  void addObject(Object3D *obj) {
    objects.push_back(obj);
    bbox.extend(obj->getBBox());
  }

  int getGroupSize() { return objects.size(); }

 protected:
  vector<Object3D *> objects;
};

#endif
