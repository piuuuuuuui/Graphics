#ifndef FRACTAL_H
#define FRACTAL_H

#include <vector>

#include "group.hpp"
#include "transform.hpp"

class Fractal : public Group {
 public:
  Fractal() = delete;

  Fractal(const vector<Affine3f> &transforms, Object3D *obj) {
    objects.push_back(obj);
    bbox = obj->getBBox();
    for (auto tr : transforms) {
      objects.push_back(new Transform(tr, this));
      vector<Eigen::Vector4f> cols;
      Eigen::EigenSolver<Matrix4f> es(tr.matrix());
      for (int i = 0; i < 4; i++) {
        if (abs(es.eigenvalues()[i] - 1.f) < 1e-5) {
          auto col = es.eigenvectors().col(i).real();
          cols.push_back(col);
        }
      }
      Eigen::MatrixXf A(4, cols.size());
      for (auto col : cols) A << col;
      Eigen::VectorXf x =
          A.colPivHouseholderQr().solve(bbox.center().homogeneous());
      Vector3f fixed = (A * x).hnormalized();
      float r = (fixed - bbox.min())
                    .cwiseAbs()
                    .cwiseMax((fixed - bbox.max()).cwiseAbs())
                    .norm();
      bbox.min() = fixed - Vector3f::Constant(r);
      bbox.max() = fixed + Vector3f::Constant(r);
    }
  }

  ~Fractal() {}
};

#endif  // FRACTAL_H