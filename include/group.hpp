#ifndef GROUP_H
#define GROUP_H

#include <iostream>
#include <vector>

#include "hit.hpp"
#include "object3d.hpp"
#include "ray.hpp"

class Group : public Object3D {
 public:
  Group() : objects(std::vector<Object3D *>()) {}

  explicit Group(int num_objects)
      : objects(std::vector<Object3D *>(num_objects)) {}

  ~Group() override {}

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    bool result = false;
    for (Object3D *object : objects)
      result |= object->intersect(r, h, obj, tmin);
    return result;
  }

  void addObject(int index, Object3D *obj) { objects[index] = obj; }

  int getGroupSize() { return objects.size(); }

 private:
  std::vector<Object3D *> objects;
};

#endif
