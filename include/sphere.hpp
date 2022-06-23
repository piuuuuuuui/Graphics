#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"

class Sphere : public Object3D {
 public:
  Sphere()
      : center(Vector3f::Zero()), radius(1.f) {}  // unit ball at the center

  Sphere(const Vector3f &center, float radius, Material *material)
      : Object3D(material), center(center), radius(radius) {
    bbox.min() = center - Vector3f::Constant(radius);
    bbox.max() = center + Vector3f::Constant(radius);
  }

  ~Sphere() override = default;

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    float rr = radius * radius;
    Vector3f vec = center - r.origin, dir = r.direction;
    float t0 = vec.dot(dir) / dir.squaredNorm();
    float dd = (dir * t0 - vec).squaredNorm();
    if (rr > dd) {
      float t = t0 - sqrt(rr - dd);  // first intersection
      if (t < h.t) {
        if (t > tmin) {
          Vector3f p = r.pointAtParameter(t), n = (p - center) / radius;
          h.t = t;
          h.u = 0.5 + atan2(n.x(), n.z()) / (2 * M_PI);
          h.v = 0.5 - asin(n.y()) / M_PI;
          h.point = p;
          h.normal = n;
          h.material = material;
          obj = this;
          return true;
        } else {
          t = t0 * 2 - t;  // second intersection
          if (t > tmin && t < h.t) {
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
        }
      }
    }
    return false;
  }

 protected:
  Vector3f center;
  float radius;
};

#endif
