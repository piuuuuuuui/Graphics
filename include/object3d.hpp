#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <cstdio>

#include "hit.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "utils.hpp"

inline bool isNegligible(const Vector3f &color) {
  return color.x() < 1e-2 && color.y() < 1e-2 && color.z() < 1e-2;
}

// Base class for all 3d entities.
class Object3D {
 public:
  Object3D() : material(nullptr) {}

  virtual ~Object3D() = default;

  explicit Object3D(Material *material) { this->material = material; }

  // Intersect Ray with this object. If hit, store information in hit structure.
  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) = 0;

  virtual bool getNextRay(const Hit &hit, Ray &ray, Vector3f &color) {
    // printf("%p %s\n", this, typeid(*this).name());
    ray.origin = hit.point;

    Vector3f &dir = ray.direction, &att = ray.attenuation;

    // update color
    if (ray.translucency > 0) {
      float c = exp(-ray.translucency * hit.t);
      att = att * c + ray.translucentColor * ((1.f - c) / ray.translucency);
    }
    color += material->emissiveColor * att;

    // Lambertian diffuse reflection
    float roughness = material->roughness;
    att =
        att * (Vector3f(1.f - roughness) + material->diffuseColor * roughness);
    if (isNegligible(att)) return false;
    Vector3f normal = getNormal(hit.normal, hit.u, hit.v);
    dir = dir * (1.f - roughness) +
          (RAND_VEC - normal).normalized() * roughness;  // to fix

    // reflection & refraction
    float n = material->refractiveIndex, lambda = material->translucency;
    if (RAND_U < material->transparency && 0 < n) {
      float cosI = -Vector3f::dot(dir, normal);
      if (cosI < 0) {
        cosI = -cosI;
        normal = -normal;
        n = 1 / n;
        lambda = -lambda;
      }
      Vector3f sinI = Vector3f::cross(dir, normal),
               sinT = Vector3f::cross(normal, sinI) / n;
      float squaredCosT = 1 - sinT.squaredLength();
      if (squaredCosT > 0) {
        float cosT = sqrt(squaredCosT),
              RS = pow((n * cosI - cosT) / (n * cosI + cosT), 2),
              RP = pow((n * cosT - cosI) / (n * cosT + cosI), 2),
              R = (RS + RP) / 2;  // Fresnel reflectivity
        if (R < RAND_U) {
          // refraction
          att = att * material->refractiveColor;
          if (isNegligible(att)) return false;
          dir = sinT - normal * cosT;
          ray.refractiveIndex *= n;
          ray.translucency += lambda;
          if (ray.translucency < 0) ray.translucency = 0;
          ray.translucentColor += lambda * material->translucentColor;
          return true;
        }
      }
    }
    // specular reflection
    att = att * material->specularColor;
    if (isNegligible(att)) return false;
    dir -= normal * 2 * Vector3f::dot(normal, dir);
    return true;
  }

 protected:
  virtual Vector3f getNormal(const Vector3f &n, float, float) { return n; }

  Material *material;
};

#endif
