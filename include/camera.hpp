#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "utils.hpp"

class Camera {
 public:
  Camera(const Vector3d &center, const Vector3d &direction, const Vector3d &up,
         int imgW, int imgH) {
    this->center = center;
    this->direction = direction.normalized();
    this->horizontal = this->direction.cross(up).normalized();
    this->up = this->horizontal.cross(this->direction);
    this->width = imgW;
    this->height = imgH;
  }

  // Generate rays for each screen-space coordinate
  virtual Ray generateRay(const Vector2d &point) = 0;
  virtual ~Camera() = default;

  int getWidth() const { return width; }
  int getHeight() const { return height; }

 protected:
  // Extrinsic parameters
  Vector3d center;
  Vector3d direction;
  Vector3d up;
  Vector3d horizontal;
  // Intrinsic parameters
  int width;
  int height;
};

class PerspectiveCamera : public Camera {
 public:
  PerspectiveCamera(const Vector3d &center, const Vector3d &direction,
                    const Vector3d &up, int imgW, int imgH, double angle)
      : Camera(center, direction, up, imgW, imgH) {
    // angle is in radian.
    o = Vector2d(width / 2., height / 2.);
    f = this->direction * (height / (tan(angle / 2) * 2));
  }

  Ray generateRay(const Vector2d &point) override {
    Vector2d op = point - o;
    return Ray(center, (f + horizontal * op.x() + up * op.y()).normalized());
  }

 protected:
  Vector2d o;  // origin, optical center
  Vector3d f;  // focus vector
};

class OrthographicCamera : public Camera {
 public:
  OrthographicCamera(const Vector3d &center, const Vector3d &direction,
                     const Vector3d &up, int imgW, int imgH, double scale = 1)
      : Camera(center, direction, up, imgW, imgH), s(scale) {
    o = Vector2d(width / 2., height / 2.);
  }

  Ray generateRay(const Vector2d &point) override {
    Vector2d op = (point - o) * this->s;
    return Ray(center + horizontal * op.x() + up * op.y(), direction);
  }

 protected:
  Vector2d o;  // origin, optical center
  double s;     // scale
};

class RealCamera : public Camera {
 public:
  RealCamera(const Vector3d &center, const Vector3d &direction,
             const Vector3d &up, int imgW, int imgH, double angle, double d,
             double a)
      : Camera(center, direction, up, imgW, imgH), dist(d), aperture(a) {
    // angle is in radian.
    o = Vector2d(width / 2., height / 2.);
    f = this->direction * (height / (tan(angle / 2) * 2));
    k = f.norm() / d;
  }

  Ray generateRay(const Vector2d &point) override {
    Vector2d op = point - o;
    Vector3d var = aperture * (horizontal * RAND_N + up * RAND_N);
    return Ray(center + var,
               (f + horizontal * op.x() + up * op.y() - var * k).normalized());
  }

 protected:
  Vector2d o;               // origin, optical center
  Vector3d f;               // focus vector
  double k, dist, aperture;  // k = f / dist
};

#endif  // CAMERA_H
