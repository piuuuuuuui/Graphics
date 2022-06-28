#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "curve.hpp"

class RevSurface : public Object3D {
 public:
  RevSurface(Curve *curve, Material *material) : Object3D(material), c(curve) {
    bbox = c->getBBox();
    rr = max(bbox.min()({0, 2}).norm(), bbox.max()({0, 2}).norm());
    bbox.min()({0, 2}) = Vector2d::Constant(-rr);
    bbox.max()({0, 2}) = Vector2d::Constant(rr);
    rr *= rr;
    newton_times = c->getControls().size() * 2;
  }

  ~RevSurface() override { delete c; }

  bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) override {
    // intersect with the bounding cylinder
    Vector2d orig = -r.origin({0, 2}), dir = r.direction({0, 2});
    double kk = dir.squaredNorm();
    double t = orig.dot(dir) / kk;
    double dd = (dir * t - orig).squaredNorm();
    if (!(dd < rr)) return false;
    tmin = max(tmin, t - sqrt((rr - dd) / kk));

    // Newton's method
    bool result = false;
    for (int i = 0; i < newton_times; ++i)
      if (newton(r, h, obj, tmin)) result = true;
    return result;
  }

  bool newton(const Ray &r, Hit &h, Object3D *&obj, double tmin) {
    // init x := (t,u,v)
    Vector3d x(tmin, RAND_U, RAND_U * (1. - DBL_EPSILON));

    // solve f(t,u,v) := p(t)-p(u,v) = 0
    Eigen::Matrix3d jacobian;       // jacobian of f
    jacobian.col(0) = r.direction;  // df/dt
    for (int i = 0; i < NEWTON_ITERS; ++i) {
      Vector3d p, tp;  // point and tangent
      compute(x[1], x[2], p, tp);
      Vector3d f = r.pointAtParameter(x[0]) - p;
      if (f.squaredNorm() < DBL_EPSILON) {
        h.t = x[0];
        h.u = x[1];
        h.v = x[2];
        h.point = p;
        h.normal = (tp.cross(Vector3d::UnitY().cross(p))).normalized();
        h.material = mtl;
        obj = this;
        return true;
      }
      jacobian.col(1) = -Vector3d::UnitY().cross(p) * (2 * M_PI);  // df/du
      jacobian.col(2) = -tp;                                       // df/dv
      x -= jacobian.inverse() * f;
      for (auto x_i : x)
        if (!isnormal(x_i)) return false;

      // boundary check
      x[0] = max(min(x[0], h.t), tmin);
      x[1] -= floor(x[1]);
      x[2] = max(min(x[2], 1. - DBL_EPSILON), 0.);
    }
    return false;
  }

  void compute(double u, double v, Vector3d &p, Vector3d &tp) {
    Affine3d rot = Affine3d::Identity();
    rot.rotate(Eigen::AngleAxisd(u * (2 * M_PI), Vector3d::UnitY()));
    c->compute(v, p, tp);
    p = rot * p;
    tp = rot.linear() * tp;
  }

 private:
  Curve *c;
  double rr;  // squared radius
  int newton_times;
};

#endif  // REVSURFACE_HPP
