#ifndef FRACTAL_H
#define FRACTAL_H

#include "group.hpp"
#include "transform.hpp"

class Fractal : public Group {
 public:
  Fractal() = delete;

  Fractal(const vector<Affine3d> &transforms, Object3D *obj) {
    objects.push_back(obj);
    bbox = obj->getBBox();
    for (auto tr : transforms) {
      objects.push_back(new Transform(tr, this));
      vector<Eigen::Vector4d> cols;
      Eigen::EigenSolver<Matrix4d> es(tr.matrix());
      for (int i = 0; i < 4; ++i) {
        if (abs(es.eigenvalues()[i] - 1.) < 1e-5) {
          auto col = es.eigenvectors().col(i).real();
          cols.push_back(col);
        }
      }
      Eigen::MatrixXd A(4, cols.size());
      for (int i = 0; i < cols.size(); ++i) A.col(i) = cols.at(i);
      Eigen::VectorXd x =
          A.colPivHouseholderQr().solve(bbox.center().homogeneous());
      Vector3d fixed = (A * x).hnormalized();
      double r = (fixed - bbox.min())
                     .cwiseAbs()
                     .cwiseMax((fixed - bbox.max()).cwiseAbs())
                     .norm();
      bbox.min() = fixed - Vector3d::Constant(r);
      bbox.max() = fixed + Vector3d::Constant(r);
    }
  }

  ~Fractal() {}
};

#endif  // FRACTAL_H