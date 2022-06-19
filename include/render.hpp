#ifndef PATH_TRACER_H
#define PATH_TRACER_H

#include <omp.h>  // OpenMP

#include <chrono>

#include "camera.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "scene_parser.hpp"
#include "utils.hpp"

using namespace std;

class PTRenderer {
 public:
  PTRenderer(const SceneParser& _scene) : scene(_scene) {}

  Image render() {
    Camera* camera = scene.getCamera();
    int w = camera->getWidth(), h = camera->getHeight();
    Image image(w, h);

    for (int i = 1; i <= NUM_ITERS; i++) {
      cout << "Iteration: " << i << ", time: ";
      auto begin = chrono::high_resolution_clock::now();
#pragma omp parallel for collapse(2) schedule(dynamic, 1)  // OpenMP
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          Ray ray = camera->generateRay(Vector2f(x + RAND_U, y + RAND_U));
          Vector3f& color = image.GetPixel(x, y);
          color = (color * i + getColor(ray)) / (i + 1);
        }
      }
      if ((i & (i - 1)) == 0) image.SaveImage("output/scene00.bmp");
      auto end = chrono::high_resolution_clock::now();
      float time_ms =
          chrono::duration_cast<chrono::nanoseconds>(end - begin).count() / 1e6;
      cout << time_ms << "ms" << endl;
    }
    return image;
  }

  Vector3f getColor(const Ray& camRay) {
    Group* group = scene.getGroup();
    Ray ray = camRay;
    Vector3f color(Vector3f::Zero());
    for (int depth = 0; depth < MAX_TRACE_DEPTH; depth++) {
      Hit hit;
      Object3D* obj = nullptr;
      if (!group->intersect(ray, hit, obj, 1e-5))
        return color + ray.attenuation.cwiseProduct(scene.getBackgroundColor());
      if (!obj->getNextRay(hit, ray, color)) break;
    }
    return color;
  }

 private:
  const SceneParser& scene;
};

#endif  // PATH_TRACER_H