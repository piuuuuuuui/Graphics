#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"

class Triangle : public Object3D {
 public:
  Triangle() = delete;

  // a b c are three vertex positions of the triangle
  Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m)
      : Object3D(m), o(a) {
    Vector3f normal = (b - a).cross(c - a).normalized();
    tr << b - a, c - a, normal;
    n.setZero();
    n.col(2) = normal;
    t.setIdentity();
    bbox.extend(a);
    bbox.extend(b);
    bbox.extend(c);
  }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    tr.col(2) = -r.direction;
    Vector3f origin = tr.inverse() * (r.origin - o);
    float u = origin.x(), v = origin.y(), t = origin.z();
    if (!(tmin < t && t < h.t && 0.f < u && 0.f < v && u + v < 1.f))
      return false;
    h.t = t;
    Vector2f uv = t * Vector2f(u, v);
    h.u = uv[0];
    h.v = uv[1];
    h.point = r.pointAtParameter(t);
    h.normal = n * Vector3f(u, v, 1);
    h.material = material;
    obj = this;
    return true;
  }

  void setVt(const Vector2f &a, const Vector2f &b, const Vector2f &c) {
    t << b - a, c - a, a;
  }

  void setVn(const Vector3f &a, const Vector3f &b, const Vector3f &c) {
    n << b - a, c - a, a;
  }

 protected:
  Vector3f o;
  Eigen::Matrix3f tr, n;
  Eigen::Matrix2f t;
};

#endif  // TRIANGLE_H
