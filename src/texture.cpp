#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const string &filename) {
  image = stbi_load(filename.c_str(), &w, &h, &c, 0);
  if (nullptr == image) cout << "Error loading " << filename << "\n";
}

VectorXd Texture::get(int x, int y) const {
  VectorXd result(c);
  result.setZero();
  if (nullptr == image) return result;
  int idx = (x + y * w) * c;
  for (int i = 0; i < c; ++i) result[i] = image[idx + i];
  return result / 255.;
}

VectorXd Texture::get(double u, double v) const {
  VectorXd result(c);
  result.setZero();
  if (nullptr == image) return result;
  u = u - floor(u);
  v = ceil(v) - v;
  double uf, vf;
  uf = modf(u * w, &u);
  vf = modf(v * h, &v);
  int ui = (int)u, vi = (int)v;
  // bilinear interpolation
  result += (1 - uf) * (1 - vf) * get(ui, vi);
  result += uf * (1 - vf) * get((ui + 1) % w, vi);
  result += (1 - uf) * vf * get(ui, (vi + 1) % h);
  result += uf * vf * get((ui + 1) % w, (vi + 1) % h);
  return result;
}