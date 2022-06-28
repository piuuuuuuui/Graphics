#ifndef MESH_H
#define MESH_H

#include <map>

#include "group.hpp"
#include "triangle.hpp"

class Mesh : public Group {
 public:
  Mesh(const string &filename, Material *m);

  Mesh(vector<Triangle *>::iterator begin, vector<Triangle *>::iterator end);

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

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override;

 protected:
  map<string, Material *> parseMtl(const string &dir, const string &filename);
};

#endif
