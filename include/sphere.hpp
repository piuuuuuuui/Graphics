#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"

class Sphere : public Object3D {
 public:
  Sphere()
      : center(Vector3f::Zero()), radius(1.f) {}  // unit ball at the center

  Sphere(const Vector3f &center, float radius, Material *material)
      : Object3D(material),
        center(center),
        radius(radius),
        rr(radius * radius) {
    bbox.min() = center - Vector3f::Constant(radius);
    bbox.max() = center + Vector3f::Constant(radius);
  }

  ~Sphere() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    Vector3f vec = center - r.origin, dir = r.direction;
    float kk = dir.squaredNorm();
    float t0 = vec.dot(dir) / kk;
    float dd = (dir * t0 - vec).squaredNorm();
    if (rr <= dd) return false;
    float t = t0 - sqrt((rr - dd) / kk);  // first intersection
    if (h.t <= t) return false;
    if (t <= tmin) {
      t = t0 * 2 - t;  // second intersection
      if (t <= tmin || h.t <= t) return false;
    }
    Vector3f p = r.pointAtParameter(t), n = (p - center) / radius;
    h.t = t;
    h.u = 0.5 + atan2(n.x(), n.z()) / (2 * M_PI);
    h.v = 0.5 - asin(n.y()) / M_PI;
    h.point = p;
    h.normal = n;
    h.material = material;
    obj = this;
    return true;
  }

 protected:
  Vector3f center;
  float radius, rr;  // rr: squared radius
};

#endif
