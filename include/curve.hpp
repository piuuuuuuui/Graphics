#ifndef CURVE_H
#define CURVE_H

#include <vector>

#include "object3d.hpp"

using Eigen::MatrixXf;

class Curve : public Object3D {
 public:
  Curve() = delete;

  Curve(const vector<Vector3f> &controls) : controls(controls) {
    for (auto point : controls) bbox.extend(point);
  }

  const vector<Vector3f> &getControls() { return controls; }

  bool intersect(const Ray &, Hit &, Object3D *&, float) override {
    return false;
  }

  virtual void compute(float t, Vector3f &p, Vector3f &tp) = 0;

 protected:
  vector<Vector3f> controls;
  vector<MatrixXf> poly_b, poly_db;
};

class BsplineCurve : public Curve {
 public:
  explicit BsplineCurve(const vector<Vector3f> &controls) : Curve(controls) {
    int n = controls.size() - 3;
    if (n <= 0) {
      printf("Number of control points of BspineCurve must be more than 3\n");
      exit(0);
    }
    MatrixXf m_points(3, n + 3);
    for (int i = 0; i < n + 3; i++) m_points.col(i) = controls.at(i);
    for (int i = 0; i < n; i++) {
      auto poly = m_points(Eigen::indexing::all, Eigen::seqN(i, 4)) * m_b;
      poly_b.push_back(poly);
      poly_db.push_back(poly * m_d);
    }
  }

  virtual void compute(float t, Vector3f &p, Vector3f &tp) {
    float i;
    t = modf(t * poly_b.size(), &i);
    Eigen::Vector4f t_pows = Eigen::Vector4f::UnitX();
    for (int j = 1; j < 4; j++) t_pows[j] = t_pows[j - 1] * t;
    p = poly_b[i] * t_pows;
    tp = poly_db[i] * t_pows.head<3>() * poly_b.size();
  }

 private:
  static const MatrixXf m_b, m_d;
};

const MatrixXf BsplineCurve::m_b({{1. / 6, -1. / 2, 1. / 2, -1. / 6},
                                  {2. / 3, 0, -1, 1. / 2},
                                  {1. / 6, 1. / 2, 1. / 2, -1. / 2},
                                  {0, 0, 0, 1. / 6}});

const MatrixXf BsplineCurve::m_d({{0, 0, 0}, {1, 0, 0}, {0, 2, 0}, {0, 0, 3}});

#endif  // CURVE_H
