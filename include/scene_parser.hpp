#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include <cassert>

#include "texture.hpp"
#include "utils.hpp"

class Camera;
class Curve;
class Disk;
class Fractal;
class Material;
class MotionBlur;
class Object3D;
class Group;
class RevSurface;
class Sphere;
class Plane;
class Portal;
class Triangle;
class Transform;
class Mesh;

#define MAX_PARSER_TOKEN_LENGTH 1024

class SceneParser {
 public:
  SceneParser() = delete;
  SceneParser(const char *filename);

  ~SceneParser();

  Camera *getCamera() const { return camera; }

  Vector3d getBackgroundColor(Vector3d dir) const {
    dir.normalize();
    Vector3d color = background_color;
    if (nullptr != background_texture)
      color = color.cwiseProduct(
          background_texture->get(0.5 + atan2(dir.x(), dir.z()) / (2 * M_PI),
                                  0.5 - asin(dir.y()) / M_PI));
    return color;
  }

  int getNumObjects() const { return num_objects; }

  Object3D *getObject(int i) const {
    assert(i >= 0 && i < num_objects);
    return objects[i];
  }

  int getNumMaterials() const { return num_materials; }

  Material *getMaterial(int i) const {
    assert(i >= 0 && i < num_materials);
    return materials[i];
  }

  Group *getGroup() const { return group; }

 private:
  void parseFile();
  void parsePerspectiveCamera();
  void parseOrthographicCamera();
  void parseRealCamera();
  void parseFisheyeCamera();
  void parseBackground();
  void parseMaterials();
  Material *parseMaterial();
  void parseObjects();
  Object3D *parseObject(char token[MAX_PARSER_TOKEN_LENGTH]);
  Group *parseGroup();
  Sphere *parseSphere();
  Plane *parsePlane();
  Disk *parseDisk();
  Triangle *parseTriangle();
  Mesh *parseTriangleMesh();
  Curve *parseBsplineCurve();
  RevSurface *parseRevSurface();
  Transform *parseTransform();
  MotionBlur *parseMotionBlur();
  Fractal *parseFractal();
  Portal *parsePortal();

  int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);

  Vector3d readVector3d();
  bool parseMatrix4d(char token[MAX_PARSER_TOKEN_LENGTH], Affine3d &tr);

  double readDouble();
  int readInt();

  FILE *file;
  Camera *camera;
  Vector3d background_color;
  Texture *background_texture;
  int num_objects;
  Object3D **objects;
  int num_materials;
  Material **materials;
  Material *current_material;
  Group *group;
};

#endif  // SCENE_PARSER_H
