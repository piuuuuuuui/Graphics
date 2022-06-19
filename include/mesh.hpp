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
    int &operator[](const int i) { return x[i]; }
    // By Computer Graphics convention, counterclockwise winding is front face
    int x[3]{};
  };

  vector<Vector3f> v;
  vector<TriangleIndex> t;
  vector<Vector3f> n;
  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override;

 private:
  // Normal can be used for light estimation
  void computeNormal();
};

#endif
