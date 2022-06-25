#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "curve.hpp"

class RevSurface : public Object3D {
 public:
  RevSurface(Curve *curve, Material *material) : Object3D(material), c(curve) {
    bbox = c->getBBox();
    rr = max(bbox.min()({0, 2}).norm(), bbox.max()({0, 2}).norm());
    bbox.min()({0, 2}) = Vector2f::Constant(-rr);
    bbox.max()({0, 2}) = Vector2f::Constant(rr);
    rr *= rr;
    newton_times = c->getControls().size() * 2;
  }

  ~RevSurface() override { delete c; }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) override {
    // intersect with the bounding cylinder
    Vector2f orig = -r.origin({0, 2}), dir = r.direction({0, 2});
    float kk = dir.squaredNorm();
    float t = orig.dot(dir) / kk;
    float dd = (dir * t - orig).squaredNorm();
    if (rr <= dd) return false;
    tmin = max(tmin, t - sqrt((rr - dd) / kk));

    // Newton's method
    bool result = false;
    for (int i = 0; i < newton_times; i++)
      if (newton(r, h, obj, tmin)) result = true;
    return result;
  }

  bool newton(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
    // init x := (t,u,v)
    Vector3f x(tmin, RAND_U, RAND_U * (1.f - 1e-6f));

    // solve f(t,u,v) := p(t)-p(u,v) = 0
    Eigen::Matrix3f jacobian;       // jacobian of f
    jacobian.col(0) = r.direction;  // df/dt
    for (int i = 0; i < NEWTON_ITERS; i++) {
      Vector3f p, tp;  // point and tangent
      compute(x[1], x[2], p, tp);
      Vector3f f = r.pointAtParameter(x[0]) - p;
      if (f.squaredNorm() < 1e-12f) {
        h.t = x[0];
        h.u = x[1];
        h.v = x[2];
        h.point = p;
        h.normal = (tp.cross(Vector3f::UnitY().cross(p))).normalized();
        h.material = material;
        obj = this;
        return true;
      }
      jacobian.col(1) = -Vector3f::UnitY().cross(p) * (2 * M_PI);  // df/du
      jacobian.col(2) = -tp;                                       // df/dv
      x -= jacobian.inverse() * f;
      for (auto x_i : x)
        if (!isnormal(x_i)) return false;

      // boundary check
      x[0] = max(min(x[0], h.t), tmin);
      x[1] -= floor(x[1]);
      x[2] = max(min(x[2], 1.f - 1e-6f), 0.f);
    }
    return false;
  }

  void compute(float u, float v, Vector3f &p, Vector3f &tp) {
    Affine3f rot = Affine3f::Identity();
    rot.rotate(Eigen::AngleAxisf(u * (2 * M_PI), Vector3f::UnitY()));
    c->compute(v, p, tp);
    p = rot * p;
    tp = rot.linear() * tp;
  }

 private:
  Curve *c;
  float rr;  // squared radius
  int newton_times;
};

#endif  // REVSURFACE_HPP
