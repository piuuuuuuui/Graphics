#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <unsupported/Eigen/MatrixFunctions>

#define RAND_U uniform(gen)
#define RAND_N gauss(gen)
#define RAND_VEC Vector3f(gauss(gen), gauss(gen), gauss(gen)).normalized()

using namespace std;
using namespace chrono;
using Eigen::Vector2f, Eigen::Vector3f, Eigen::Vector4f, Eigen::Affine3f,
    Eigen::Matrix4f, Eigen::MatrixPower, Eigen::AngleAxisf, Eigen::AlignedBox3f;

const int NUM_ITERS = 256;
const int MAX_DEPTH = 16;
const float FPS = NUM_ITERS / 1.f;

static random_device rd;
static mt19937 gen(rd());
static uniform_real_distribution<float> uniform(0.0, 1.0);
static normal_distribution<float> gauss(0.0, 1.0);

#endif  // UTILS_H
