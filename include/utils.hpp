#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <unsupported/Eigen/MatrixFunctions>
#include <vector>

#define DBL_LARGE (1. / DBL_EPSILON)
#define RAND_U uniform(gen)
#define RAND_N gauss(gen)
#define RAND_VEC Vector3d(gauss(gen), gauss(gen), gauss(gen)).normalized()

using namespace std;
using namespace chrono;
using Eigen::Vector2d, Eigen::Vector3d, Eigen::Affine3d, Eigen::Matrix4d;

const int NUM_ITERS = 1024;
const int MAX_DEPTH = 16;
const double T_MIN = 1e-5;
const double FPS = NUM_ITERS / 1.;
const int NEWTON_ITERS = 16;

extern int iter;

static random_device rd;
static mt19937 gen(rd());
static uniform_real_distribution<double> uniform(0.0, 1.0);
static normal_distribution<double> gauss(0.0, 1.0);

template <typename T>
vector<size_t> argsort(const vector<T> &v) {
  vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);
  stable_sort(idx.begin(), idx.end(),
              [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });
  return idx;
}

#endif  // UTILS_H
