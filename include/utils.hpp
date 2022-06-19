#ifndef UTILS_H
#define UTILS_H

#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> uniform(0.0, 1.0);
static std::normal_distribution<float> gauss(0.0, 1.0);

#define RAND_U uniform(gen)
#define RAND_N gauss(gen)
#define RAND_VEC Vector3f(gauss(gen), gauss(gen), gauss(gen)).normalized()

const int NUM_ITERS = 256;
const int MAX_TRACE_DEPTH = 16;

#endif  // UTILS_H
