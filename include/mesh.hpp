#ifndef MESH_H
#define MESH_H

#include <vector>

#include "triangle.hpp"

class Mesh : public Object3D {
 public:
  Mesh(const char *filename, Material *m);

  struct TriangleIndex {
    TriangleIndex() {
      x[0] = 0;
      x[1] = 0;
      x[2] = 0;
    }

    int &operator[](int i) { return x[i]; }

    const int operator[](int i) const { return x[i]; }

    bool valid() { return 0 <= x[0] && 0 <= x[1] && 0 <= x[2]; }

    // By Computer Graphics convention, counterclockwise winding is front face
    int x[3];
  };

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override;

 protected:
  vector<Triangle *> triangles;
};

#endif
