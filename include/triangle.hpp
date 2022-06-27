#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"

class Triangle : public Object3D {
 public:
  Triangle() = delete;

  // a b c are three vertex positions of the triangle
  Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m)
      : Object3D(m), o(a) {
    tr << b - a, c - a;
    normal = (b - a).cross(c - a).normalized();
    bbox.min() = a.cwiseMin(b.cwiseMin(c));
    bbox.max() = a.cwiseMax(b.cwiseMax(c));
  }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    tr.col(2) = -r.direction;
    Vector3f origin = tr.inverse() * (r.origin - o);
    float u = origin.x(), v = origin.y(), t = origin.z();
    if (!(tmin < t && t < h.t && 0.f < u && 0.f < v && u + v < 1.f))
      return false;
    h.t = t;
    h.u = u;
    h.v = v;
    h.point = r.pointAtParameter(t);
    h.normal = normal;
    h.material = material;
    obj = this;
    return true;
  }

  Vector3f normal;

 protected:
  Vector3f o;
  Eigen::Matrix3f tr;
};

#endif  // TRIANGLE_H
