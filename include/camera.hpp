#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "utils.hpp"

class Camera {
 public:
  Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up,
         int imgW, int imgH) {
    this->center = center;
    this->direction = direction.normalized();
    this->horizontal = this->direction.cross(up).normalized();
    this->up = this->horizontal.cross(this->direction);
    this->width = imgW;
    this->height = imgH;
  }

  // Generate rays for each screen-space coordinate
  virtual Ray generateRay(const Vector2f &point) = 0;
  virtual ~Camera() = default;

  int getWidth() const { return width; }
  int getHeight() const { return height; }

 protected:
  // Extrinsic parameters
  Vector3f center;
  Vector3f direction;
  Vector3f up;
  Vector3f horizontal;
  // Intrinsic parameters
  int width;
  int height;
};

class PerspectiveCamera : public Camera {
 public:
  PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
                    const Vector3f &up, int imgW, int imgH, float angle)
      : Camera(center, direction, up, imgW, imgH) {
    // angle is in radian.
    o = Vector2f(width / 2.f, height / 2.f);
    f = this->direction * (height / (tan(angle / 2) * 2));
  }

  Ray generateRay(const Vector2f &point) override {
    Vector2f op = point - o;
    return Ray(center, (f + horizontal * op.x() + up * op.y()).normalized());
  }

 protected:
  Vector2f o;  // origin, optical center
  Vector3f f;  // focus vector
};

class OrthographicCamera : public Camera {
 public:
  OrthographicCamera(const Vector3f &center, const Vector3f &direction,
                     const Vector3f &up, int imgW, int imgH, float scale = 1)
      : Camera(center, direction, up, imgW, imgH), s(scale) {
    o = Vector2f(width / 2.f, height / 2.f);
  }

  Ray generateRay(const Vector2f &point) override {
    Vector2f op = (point - o) * this->s;
    return Ray(center + horizontal * op.x() + up * op.y(), direction);
  }

 protected:
  Vector2f o;  // origin, optical center
  float s;     // scale
};

class RealCamera : public Camera {
 public:
  RealCamera(const Vector3f &center, const Vector3f &direction,
             const Vector3f &up, int imgW, int imgH, float angle, float d,
             float a)
      : Camera(center, direction, up, imgW, imgH), dist(d), aperture(a) {
    // angle is in radian.
    o = Vector2f(width / 2.f, height / 2.f);
    f = this->direction * (height / (tan(angle / 2) * 2));
    k = f.norm() / d;
  }

  Ray generateRay(const Vector2f &point) override {
    Vector2f op = point - o;
    Vector3f var = aperture * (horizontal * RAND_N + up * RAND_N);
    return Ray(center + var,
               (f + horizontal * op.x() + up * op.y() - var * k).normalized());
  }

 protected:
  Vector2f o;               // origin, optical center
  Vector3f f;               // focus vector
  float k, dist, aperture;  // k = f / dist
};

#endif  // CAMERA_H
