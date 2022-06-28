#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"

class Triangle : public Object3D {
 public:
  Triangle() = delete;

  // a b c are three vertex positions of the triangle
  Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c, Material *m)
      : Object3D(m), o(a) {
    Vector3d normal = (b - a).cross(c - a).normalized();
    tr << b - a, c - a, normal;
    vn.setZero();
    vn.col(2) = normal;
    vt.setIdentity();
    bbox.extend(a);
    bbox.extend(b);
    bbox.extend(c);
  }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override {
    tr.col(2) = -r.direction;
    Vector3d origin = tr.inverse() * (r.origin - o);
    double u = origin.x(), v = origin.y(), t = origin.z();
    if (!(tmin < t && t < h.t && 0. < u && 0. < v && u + v < 1.)) return false;
    h.t = t;
    Vector2d uv = vt * Vector2d(u, v);
    h.u = uv[0];
    h.v = uv[1];
    h.point = r.pointAtParameter(t);
    h.normal = vn * Vector3d(u, v, 1);
    h.material = mtl;
    obj = this;
    return true;
  }

  void setVt(const Vector2d &a, const Vector2d &b, const Vector2d &c) {
    vt.matrix().topRows<2>() << b - a, c - a, a;
  }

  void setVn(const Vector3d &a, const Vector3d &b, const Vector3d &c) {
    vn << b - a, c - a, a;
  }

 protected:
  Vector3d o;
  Eigen::Matrix3d tr, vn;
  Eigen::Affine2d vt;
};

#endif  // TRIANGLE_H
