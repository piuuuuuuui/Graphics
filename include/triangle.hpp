#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"

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
    normal = (b - a).cross(c - a).normalized();
    d = normal.dot(a);
    bbox.min() = a.cwiseMin(b.cwiseMin(c));
    bbox.max() = a.cwiseMax(b.cwiseMax(c));
  }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    float cos = -normal.dot(r.direction);
    float t = (normal.dot(r.origin) - d) / cos;
    if (t <= tmin || h.t <= t) return false;
    Vector3f p = r.pointAtParameter(t);
    Vector3f a = vertices[0] - p, b = vertices[1] - p, c = vertices[2] - p;
    if (normal.dot(a.cross(b)) < 0 || normal.dot(b.cross(c)) < 0 ||
        normal.dot(c.cross(a)) < 0)
      return false;
    h.t = t;
    h.point = p;
    h.normal = normal;
    h.material = material;
    obj = this;
    return true;
  }

  Vector3f normal;
  Vector3f vertices[3];

 protected:
  float d;
};

#endif  // TRIANGLE_H
