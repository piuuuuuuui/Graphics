#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vecmath.h>

#include <cmath>
#include <iostream>

#include "object3d.hpp"
using namespace std;

class Triangle : public Object3D {
 public:
  Triangle() = delete;

  // a b c are three vertex positions of the triangle
  Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m)
      : Object3D(m) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
    // counterclockwise orientation
    normal = Vector3f::cross(b - a, c - a).normalized();
    d = Vector3f::dot(normal, a);
  }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    float cos = -Vector3f::dot(normal, r.direction);
    float t = (Vector3f::dot(normal, r.origin) - d) / cos;
    if (t < h.t && t > tmin) {
      Vector3f p = r.pointAtParameter(t);
      Vector3f a = vertices[0] - p, b = vertices[1] - p, c = vertices[2] - p;
      if (Vector3f::dot(normal, Vector3f::cross(a, b)) >= 0 &&
          Vector3f::dot(normal, Vector3f::cross(b, c)) >= 0 &&
          Vector3f::dot(normal, Vector3f::cross(c, a)) >= 0) {
        h.t = t;
        h.point = r.pointAtParameter(t);
        h.normal = normal;
        h.material = material;
        obj = this;
        return true;
      }
    }
    return false;
  }

  Vector3f normal;
  Vector3f vertices[3];

 protected:
  float d;
};

#endif  // TRIANGLE_H
