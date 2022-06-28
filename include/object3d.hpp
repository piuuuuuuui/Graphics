#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "hit.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "utils.hpp"

class AABB : public Eigen::AlignedBox3d {
 public:
  AABB() = default;

  AABB(const Eigen::AlignedBox3d &bbox) : Eigen::AlignedBox3d(bbox) {}

  ~AABB() = default;

  /** \returns true if r intersects with *this within (tmin, tmax).
   * Also updates tmin and tmax. */
  inline bool intersect(const Ray &r, double &tmin, double &tmax) const {
    for (int i = 0; i < 3; ++i) {
      double dir_i = r.direction[i], t1, t2;
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
  Object3D() : mtl(nullptr) {}

  virtual ~Object3D() = default;

  explicit Object3D(Material *material) : mtl(material) {}

  // Intersect Ray with this object. If hit, store information in hit structure.
  virtual bool intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) = 0;

  virtual bool getNextRay(const Hit &hit, Ray &ray) {
    Vector3d &dir = ray.direction;
    Affine3d &tr = ray.colorTrans;

    Material material = getMaterial(hit.material, hit.u, hit.v);
    Vector3d normal = getNormal(hit.normal, hit.u, hit.v);

    double roughness = material.roughness;
    double n = material.refractiveIndex;
    Matrix4d m = material.translucency;

    // orient normal and others
    double cosI = -dir.dot(normal);
    if (cosI < 0) {
      cosI = -cosI;
      normal = -normal;
      n = 1 / n;
      m = -m;
    }

    ray.origin = hit.point;

    // update color
    tr *= (ray.translucency * hit.t).exp();
    tr.translate(material.emissiveColor);

    // Lambertian diffuse reflection
    tr.scale(Vector3d::Constant(1. - roughness) +
             material.diffuseColor * roughness);
    if ((tr.linear() * Vector3d::Ones()).maxCoeff() < 1e-2) return false;
    dir = dir * (1. - roughness) +
          (RAND_VEC - normal).normalized() * roughness;  // TODO: fix

    // reflection & refraction
    if (RAND_U < material.transparency && 0 < n) {
      Vector3d sinI = dir.cross(normal), sinT = normal.cross(sinI) / n;
      double squaredCosT = 1 - sinT.squaredNorm();
      if (0 < squaredCosT) {
        double cosT = sqrt(squaredCosT),
               RS = pow((n * cosI - cosT) / (n * cosI + cosT), 2),
               RP = pow((n * cosT - cosI) / (n * cosT + cosI), 2),
               R = (RS + RP) / 2;  // Fresnel reflectivity
        if (R < RAND_U) {
          // refraction
          tr.scale(material.refractiveColor);
          if ((tr.linear() * Vector3d::Ones()).maxCoeff() < 1e-2) return false;
          dir = sinT - normal * cosT;
          ray.refractiveIndex *= n;
          ray.translucency += m;
          return true;
        }
      }
    }
    // specular reflection
    tr.scale(material.specularColor);
    if ((tr.linear() * Vector3d::Ones()).maxCoeff() < 1e-2) return false;
    dir -= normal * 2 * normal.dot(dir);
    return true;
  }

  const AABB &getBBox() const { return bbox; }

 protected:
  virtual Vector3d getNormal(const Vector3d &normal, double u, double v) {
    Vector3d n = normal;
    // TODO
    return n;
  }

  virtual Material getMaterial(const Material *material, double u, double v) {
    Material m = *material;
    // TODO
    return m;
  }

  Material *mtl;
  AABB bbox;
};

#endif
