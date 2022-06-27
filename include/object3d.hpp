#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "hit.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "utils.hpp"

class AABB : public Eigen::AlignedBox3f {
 public:
  AABB() = default;

  AABB(const Eigen::AlignedBox3f &bbox) : Eigen::AlignedBox3f(bbox) {}

  ~AABB() = default;

  /** \returns true if r intersects with *this within (tmin, tmax).
   * Also updates tmin and tmax. */
  inline bool intersect(const Ray &r, float &tmin, float &tmax) {
    for (int i = 0; i < 3; i++) {
      float dir_i = r.direction[i], t1, t2;
      if (0 <= dir_i) {
        t1 = (min()[i] - r.origin[i]) / dir_i;
        t2 = (max()[i] - r.origin[i]) / dir_i;
      } else {
        t1 = (max()[i] - r.origin[i]) / dir_i;
        t2 = (min()[i] - r.origin[i]) / dir_i;
      }
      tmin = std::max(tmin, t1);
      tmax = std::min(tmax, t2);
      if (!(tmin < tmax)) return false;
    }
    return true;
  }
};

// Base class for all 3d entities.
class Object3D {
 public:
  Object3D() : material(nullptr) {}

  virtual ~Object3D() = default;

  explicit Object3D(Material *material) { this->material = material; }

  // Intersect Ray with this object. If hit, store information in hit structure.
  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) = 0;

  virtual bool getNextRay(const Hit &hit, Ray &ray) {
    Vector3f &dir = ray.direction;
    Affine3f &tr = ray.colorTrans;
    Vector3f normal = getNormal(hit.normal, hit.u, hit.v);
    float roughness = material->roughness;
    float n = material->refractiveIndex;
    Matrix4f m = material->translucency;

    // orient normal and others
    float cosI = -dir.dot(normal);
    if (cosI < 0) {
      cosI = -cosI;
      normal = -normal;
      n = 1 / n;
      m = -m;
    }

    ray.origin = hit.point;

    // update color
    tr *= (ray.translucency * hit.t).exp();
    tr.translate(material->emissiveColor);

    // Lambertian diffuse reflection
    tr.scale(Vector3f::Constant(1.f - roughness) +
             material->diffuseColor * roughness);
    if ((tr.linear() * Vector3f::Ones()).maxCoeff() < 1e-2f) return false;
    dir = dir * (1.f - roughness) +
          (RAND_VEC - normal).normalized() * roughness;  // TODO: fix

    // reflection & refraction
    if (RAND_U < material->transparency && 0 < n) {
      Vector3f sinI = dir.cross(normal), sinT = normal.cross(sinI) / n;
      float squaredCosT = 1 - sinT.squaredNorm();
      if (0 < squaredCosT) {
        float cosT = sqrt(squaredCosT),
              RS = pow((n * cosI - cosT) / (n * cosI + cosT), 2),
              RP = pow((n * cosT - cosI) / (n * cosT + cosI), 2),
              R = (RS + RP) / 2;  // Fresnel reflectivity
        if (R < RAND_U) {
          // refraction
          tr.scale(material->refractiveColor);
          if ((tr.linear() * Vector3f::Ones()).maxCoeff() < 1e-2f) return false;
          dir = sinT - normal * cosT;
          ray.refractiveIndex *= n;
          ray.translucency += m;
          return true;
        }
      }
    }
    // specular reflection
    tr.scale(material->specularColor);
    if ((tr.linear() * Vector3f::Ones()).maxCoeff() < 1e-2f) return false;
    dir -= normal * 2 * normal.dot(dir);
    return true;
  }

  const AABB &getBBox() const { return bbox; }

 protected:
  virtual Vector3f getNormal(const Vector3f &n, float, float) { return n; }

  Material *material;
  AABB bbox;
};

#endif
