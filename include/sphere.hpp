#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"

class Sphere : public Object3D {
 public:
  Sphere() : center(Vector3d::Zero()), radius(1.) {}  // unit ball at the center

  Sphere(const Vector3d &center, double radius, Material *material)
      : Object3D(material),
        center(center),
        radius(radius),
        rr(radius * radius) {
    bbox.min() = center - Vector3d::Constant(radius);
    bbox.max() = center + Vector3d::Constant(radius);
  }

  ~Sphere() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override {
    Vector3d vec = center - r.origin, dir = r.direction;
    double kk = dir.squaredNorm();
    double t0 = vec.dot(dir) / kk;
    double dd = (dir * t0 - vec).squaredNorm();
    if (!(dd < rr)) return false;
    double t = t0 - sqrt((rr - dd) / kk);  // first intersection
    if (!(t < h.t)) return false;
    if (!(tmin < t)) {
      t = t0 * 2 - t;  // second intersection
      if (!(tmin < t && t < h.t)) return false;
    }
    Vector3d p = r.pointAtParameter(t), n = (p - center) / radius;
    h.t = t;
    h.u = 0.5 + atan2(n.x(), n.z()) / (2 * M_PI);
    h.v = acos(n.y()) / M_PI;
    h.point = p;
    h.normal = n;
    h.material = mtl;
    obj = this;
    return true;
  }

 protected:
  virtual Vector3d getTangent(const Vector2d &dir, double u,
                              double v) override {
    Vector3d t = Vector3d::Zero();
    t.head<2>() = dir;
    u *= 2 * M_PI;
    v *= M_PI;
    Vector3d p = Vector3d(-sin(u) * sin(v), cos(v), -cos(u) * sin(v)) * radius,
             tu = Vector3d::UnitY().cross(p) * (2 * M_PI),
             tv = tu.normalized().cross(p) * M_PI;
    Eigen::Matrix3d tr;
    tr << tu, tv, p;
    return tr.inverse().transpose() * t;
  }

  Vector3d center;
  double radius, rr;  // rr: squared radius
};

#endif
