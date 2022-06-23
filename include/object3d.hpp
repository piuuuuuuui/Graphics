#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "hit.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "utils.hpp"

// Base class for all 3d entities.
class Object3D {
 public:
  Object3D() : material(nullptr) {}

  virtual ~Object3D() = default;

  explicit Object3D(Material *material) { this->material = material; }

  // Intersect Ray with this object. If hit, store information in hit structure.
  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
    // check if intersect with bbox
    // should not change h and obj
    float tmax = FLT_MAX;
    for (int i = 0; i < 3; i++) {
      float t1 = (bbox.min()[i] - r.origin[i]) / r.direction[i],
            t2 = (bbox.max()[i] - r.origin[i]) / r.direction[i];
      tmin = max(tmin, min(t1, t2));
      tmax = min(tmax, max(t1, t2));
    }
    return tmin < tmax;
  }

  virtual bool getNextRay(const Hit &hit, Ray &ray) {
    ray.origin = hit.point;

    Vector3f &dir = ray.direction;
    Affine3f &tr = ray.colorTrans;

    // update color
    tr *= (ray.translucency * hit.t).exp();
    tr.translate(material->emissiveColor);

    // Lambertian diffuse reflection
    float roughness = material->roughness;
    tr.scale(Vector3f::Constant(1.f - roughness) +
             material->diffuseColor * roughness);
    if ((tr.linear() * Vector3f::Ones()).maxCoeff() < 1e-2) return false;
    Vector3f normal = getNormal(hit.normal, hit.u, hit.v);
    dir = dir * (1.f - roughness) +
          (RAND_VEC - normal).normalized() * roughness;  // TODO: fix

    // reflection & refraction
    float n = material->refractiveIndex;
    Matrix4f m = material->translucency;
    if (RAND_U < material->transparency && 0 < n) {
      float cosI = -dir.dot(normal);
      if (cosI < 0) {
        cosI = -cosI;
        normal = -normal;
        n = 1 / n;
        m = -m;
      }
      Vector3f sinI = dir.cross(normal), sinT = normal.cross(sinI) / n;
      float squaredCosT = 1 - sinT.squaredNorm();
      if (squaredCosT > 0) {
        float cosT = sqrt(squaredCosT),
              RS = pow((n * cosI - cosT) / (n * cosI + cosT), 2),
              RP = pow((n * cosT - cosI) / (n * cosT + cosI), 2),
              R = (RS + RP) / 2;  // Fresnel reflectivity
        if (R < RAND_U) {
          // refraction
          tr.scale(material->refractiveColor);
          if ((tr.linear() * Vector3f::Ones()).maxCoeff() < 1e-2) return false;
          dir = sinT - normal * cosT;
          ray.refractiveIndex *= n;
          ray.translucency += m;
          return true;
        }
      }
    }
    // specular reflection
    tr.scale(material->specularColor);
    if ((tr.linear() * Vector3f::Ones()).maxCoeff() < 1e-2) return false;
    dir -= normal * 2 * normal.dot(dir);
    return true;
  }

  const Eigen::AlignedBox3f &getBBox() const { return bbox; }

 protected:
  virtual Vector3f getNormal(const Vector3f &n, float, float) { return n; }

  Material *material;
  Eigen::AlignedBox3f bbox;
};

#endif
