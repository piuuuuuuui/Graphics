#ifndef GROUP_H
#define GROUP_H

#include <vector>

#include "object3d.hpp"

class Group : public Object3D {
 public:
  Group() : objects(vector<Object3D *>()) {}

  explicit Group(int num_objects) : objects(vector<Object3D *>(num_objects)) {}

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
  vector<Object3D *> objects;
};

#endif
