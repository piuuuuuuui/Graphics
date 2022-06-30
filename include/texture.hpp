#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include "utils.hpp"

using Eigen::VectorXd;

class Texture {
 public:
  Texture(const string &filename);

  VectorXd get(int u, int v) const;

  VectorXd get(double u, double v) const;

  Vector2d getGrad(double u, double v) const;

 private:
  unsigned char *image;
  int w, h, c;
};

#endif  // !TEXTURE_H