#ifndef FRACTAL_H
#define FRACTAL_H

#include <vector>

#include "group.hpp"
#include "transform.hpp"

class Fractal : public Group {
 public:
  Fractal() = delete;

  Fractal(const vector<Affine3f> &transforms, Object3D *obj) {
    addObject(obj);
    bbox = obj->getBBox();
    for (auto tr : transforms) {
      addObject(new Transform(tr, this));
      vector<Vector3f> cols;
      Eigen::EigenSolver<Matrix4f> es(tr.matrix());
      for (int i = 0; i < 4; i++) {
        if (abs(es.eigenvalues()[i] - 1.f) < 1e-5) {
          auto col = es.eigenvectors().col(i).real();
          cols.push_back(col.head<3>() / (col[3] + 1e-9));
        }
      }
      Eigen::MatrixXf A(3, cols.size());
      for (auto col : cols) A << col;
      Eigen::VectorXf x = A.colPivHouseholderQr().solve(bbox.center());
      bbox.extend(A * x / x.sum());
    }
  }

  ~Fractal() {}
};

#endif  // FRACTAL_H