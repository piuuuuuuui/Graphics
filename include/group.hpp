#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"

class Group : public Object3D {
 public:
  Group() {}

  ~Group() override {}

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override {
    vector<double> tmins;
    for (int i = 0; i < num_objects; ++i) {
      double t1 = tmin, t2 = h.t;
      if (!objects[i]->getBBox().intersect(r, t1, t2))
        tmins.push_back(DBL_MAX);
      else
        tmins.push_back(t1);
    }
    bool result = false;
    for (int i : argsort(tmins)) {
      if (!(tmins[i] < h.t)) break;
      result |= objects[i]->intersect(r, h, obj, tmin);
    }
    return result;
  }

  void addObject(Object3D *obj) {
    objects.push_back(obj);
    ++num_objects;
    bbox.extend(obj->getBBox());
  }

  int getGroupSize() { return objects.size(); }

 protected:
  vector<Object3D *> objects;
  int num_objects = 0;
};

#endif
