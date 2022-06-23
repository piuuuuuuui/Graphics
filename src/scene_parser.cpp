#include "scene_parser.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "camera.hpp"
#include "disk.hpp"
#include "fractal.hpp"
#include "group.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "object3d.hpp"
#include "plane.hpp"
#include "portal.hpp"
#include "sphere.hpp"
#include "transform.hpp"
#include "triangle.hpp"

#define Deg2Rad(x) ((M_PI * x) / 180.0f)

SceneParser::SceneParser(const char *filename) {
  // initialize some reasonable default values
  group = nullptr;
  camera = nullptr;
  background_color = Vector3f(0.5, 0.5, 0.5);
  num_objects = 0;
  objects = nullptr;
  num_materials = 0;
  materials = nullptr;
  current_material = nullptr;

  // parse the file
  assert(filename != nullptr);
  const char *ext = &filename[strlen(filename) - 4];

  if (strcmp(ext, ".txt") != 0) {
    printf("wrong file name extension\n");
    exit(0);
  }
  file = fopen(filename, "r");

  if (file == nullptr) {
    printf("cannot open scene file\n");
    exit(0);
  }
  parseFile();
  fclose(file);
  file = nullptr;
}

SceneParser::~SceneParser() {
  delete group;
  delete camera;

  int i;
  for (i = 0; i < num_materials; i++) {
    delete materials[i];
  }
  delete[] materials;
  delete[] objects;
}

// ====================================================================
// ====================================================================

void SceneParser::parseFile() {
  //
  // at the top level, the scene can have a camera,
  // background color and a group of objects
  //
  char token[MAX_PARSER_TOKEN_LENGTH];
  while (getToken(token)) {
    if (!strcmp(token, "PerspectiveCamera")) {
      parsePerspectiveCamera();
    } else if (!strcmp(token, "OrthographicCamera")) {
      parseOrthographicCamera();
    } else if (!strcmp(token, "RealCamera")) {
      parseRealCamera();
    } else if (!strcmp(token, "Background")) {
      parseBackground();
    } else if (!strcmp(token, "Objects")) {
      parseObjects();
    } else if (!strcmp(token, "Materials")) {
      parseMaterials();
    } else if (!strcmp(token, "Group")) {
      group = parseGroup();
    } else {
      printf("Unknown token in parseFile: '%s'\n", token);
      exit(0);
    }
  }
}

// ====================================================================
// ====================================================================

void SceneParser::parsePerspectiveCamera() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the camera parameters
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "center"));
  Vector3f center = readVector3f();
  getToken(token);
  assert(!strcmp(token, "direction"));
  Vector3f direction = readVector3f();
  getToken(token);
  assert(!strcmp(token, "up"));
  Vector3f up = readVector3f();
  getToken(token);
  assert(!strcmp(token, "angle"));
  float angle_degrees = readFloat();
  float angle_radians = Deg2Rad(angle_degrees);
  getToken(token);
  assert(!strcmp(token, "width"));
  int width = readInt();
  getToken(token);
  assert(!strcmp(token, "height"));
  int height = readInt();
  getToken(token);
  assert(!strcmp(token, "}"));
  camera = new PerspectiveCamera(center, direction, up, width, height,
                                 angle_radians);
}

void SceneParser::parseOrthographicCamera() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the camera parameters
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "center"));
  Vector3f center = readVector3f();
  getToken(token);
  assert(!strcmp(token, "direction"));
  Vector3f direction = readVector3f();
  getToken(token);
  assert(!strcmp(token, "up"));
  Vector3f up = readVector3f();
  getToken(token);
  assert(!strcmp(token, "scale"));
  float scale = readFloat();
  getToken(token);
  assert(!strcmp(token, "width"));
  int width = readInt();
  getToken(token);
  assert(!strcmp(token, "height"));
  int height = readInt();
  getToken(token);
  assert(!strcmp(token, "}"));
  camera = new OrthographicCamera(center, direction, up, width, height, scale);
}

void SceneParser::parseRealCamera() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the camera parameters
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "center"));
  Vector3f center = readVector3f();
  getToken(token);
  assert(!strcmp(token, "direction"));
  Vector3f direction = readVector3f();
  getToken(token);
  assert(!strcmp(token, "up"));
  Vector3f up = readVector3f();
  getToken(token);
  assert(!strcmp(token, "angle"));
  float angle_degrees = readFloat();
  float angle_radians = Deg2Rad(angle_degrees);
  getToken(token);
  assert(!strcmp(token, "dist"));
  float dist = readFloat();
  getToken(token);
  assert(!strcmp(token, "aperture"));
  float aperture = readFloat();
  getToken(token);
  assert(!strcmp(token, "width"));
  int width = readInt();
  getToken(token);
  assert(!strcmp(token, "height"));
  int height = readInt();
  getToken(token);
  assert(!strcmp(token, "}"));
  camera = new RealCamera(center, direction, up, width, height, angle_radians,
                          dist, aperture);
}

void SceneParser::parseBackground() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  // read in the background color
  getToken(token);
  assert(!strcmp(token, "{"));
  while (true) {
    getToken(token);
    if (!strcmp(token, "}")) {
      break;
    } else if (!strcmp(token, "color")) {
      background_color = readVector3f();
    } else {
      printf("Unknown token in parseBackground: '%s'\n", token);
      assert(0);
    }
  }
}

// ====================================================================
// ====================================================================

void SceneParser::parseObjects() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));
  // read in the number of objects
  getToken(token);
  assert(!strcmp(token, "numObjects"));
  num_objects = readInt();
  objects = new Object3D *[num_objects];
  // read in the objects
  int count = 0;
  while (num_objects > count) {
    getToken(token);
    if (!strcmp(token, "MaterialIndex")) {
      // change the current material
      int index = readInt();
      assert(index >= 0 && index < getNumMaterials());
      current_material = getMaterial(index);
    } else {
      objects[count] = parseObject(token);
      count++;
    }
  }
  getToken(token);
  assert(!strcmp(token, "}"));
}

Object3D *SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH]) {
  Object3D *answer = nullptr;
  if (!strcmp(token, "Group")) {
    answer = (Object3D *)parseGroup();
  } else if (!strcmp(token, "Sphere")) {
    answer = (Object3D *)parseSphere();
  } else if (!strcmp(token, "Plane")) {
    answer = (Object3D *)parsePlane();
  } else if (!strcmp(token, "Disk")) {
    answer = (Object3D *)parseDisk();
  } else if (!strcmp(token, "Triangle")) {
    answer = (Object3D *)parseTriangle();
  } else if (!strcmp(token, "TriangleMesh")) {
    answer = (Object3D *)parseTriangleMesh();
  } else if (!strcmp(token, "Transform")) {
    answer = (Object3D *)parseTransform();
  } else if (!strcmp(token, "MotionBlur")) {
    answer = (Object3D *)parseMotionBlur();
  } else if (!strcmp(token, "Fractal")) {
    answer = (Object3D *)parseFractal();
  } else if (!strcmp(token, "Portal")) {
    answer = (Object3D *)parsePortal();
  } else if (!strcmp(token, "ObjectIndex")) {
    int index = readInt();
    assert(index >= 0 && index < getNumObjects());
    answer = getObject(index);
  } else {
    printf("Unknown token in parseObject: '%s'\n", token);
    exit(0);
  }
  return answer;
}

// ====================================================================
// ====================================================================

void SceneParser::parseMaterials() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));
  // read in the number of objects
  getToken(token);
  assert(!strcmp(token, "numMaterials"));
  num_materials = readInt();
  if (num_materials == 0) {
    printf("No materials specified\n");
    exit(0);
  }
  materials = new Material *[num_materials];
  // read in the objects
  int count = 0;
  while (num_materials > count) {
    getToken(token);
    if (!strcmp(token, "Material") || !strcmp(token, "PhongMaterial")) {
      materials[count] = parseMaterial();
    } else {
      printf("Unknown token in parseMaterial: '%s'\n", token);
      exit(0);
    }
    count++;
  }
  getToken(token);
  assert(!strcmp(token, "}"));
  current_material = materials[0];
}

Material *SceneParser::parseMaterial() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  char filename[MAX_PARSER_TOKEN_LENGTH];
  filename[0] = 0;
  auto *answer = new Material;
  getToken(token);
  assert(!strcmp(token, "{"));
  while (true) {
    getToken(token);
    if (strcmp(token, "emissiveColor") == 0) {
      answer->emissiveColor = readVector3f();
    } else if (strcmp(token, "diffuseColor") == 0) {
      answer->diffuseColor = readVector3f();
    } else if (strcmp(token, "roughness") == 0) {
      answer->roughness = readFloat();
    } else if (strcmp(token, "specularColor") == 0) {
      answer->specularColor = readVector3f();
    } else if (strcmp(token, "transparency") == 0) {
      answer->transparency = readFloat();
    } else if (strcmp(token, "refractiveIndex") == 0) {
      answer->refractiveIndex = readFloat();
    } else if (strcmp(token, "refractiveColor") == 0) {
      answer->refractiveColor = readVector3f();
    } else if (strcmp(token, "translucency") == 0) {
      getToken(token);
      assert(!strcmp(token, "{"));
      getToken(token);
      Affine3f tr(Affine3f::Identity());
      while (parseMatrix4f(token, tr)) getToken(token);
      assert(!strcmp(token, "}"));
      answer->translucency = tr.matrix().log();
    } else if (strcmp(token, "texture") == 0) {
      // Optional: read in texture and draw it.
      getToken(filename);
    } else {
      assert(!strcmp(token, "}"));
      break;
    }
  }
  return answer;
}

// ====================================================================
// ====================================================================

Group *SceneParser::parseGroup() {
  //
  // each group starts with an integer that specifies
  // the number of objects in the group
  //
  // the material index sets the material of all objects which follow,
  // until the next material index (scoping for the materials is very
  // simple, and essentially ignores any tree hierarchy)
  //
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));

  // read in the number of objects
  getToken(token);
  assert(!strcmp(token, "numObjects"));
  int num_group_objects = readInt();

  auto *answer = new Group;

  // read in the objects
  int count = 0;
  while (num_group_objects > count) {
    getToken(token);
    if (!strcmp(token, "MaterialIndex")) {
      // change the current material
      int index = readInt();
      assert(index >= 0 && index < getNumMaterials());
      current_material = getMaterial(index);
    } else {
      Object3D *object = parseObject(token);
      assert(object != nullptr);
      answer->addObject(object);
      count++;
    }
  }
  getToken(token);
  assert(!strcmp(token, "}"));

  // return the group
  return answer;
}

// ====================================================================
// ====================================================================

Sphere *SceneParser::parseSphere() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "center"));
  Vector3f center = readVector3f();
  getToken(token);
  assert(!strcmp(token, "radius"));
  float radius = readFloat();
  getToken(token);
  assert(!strcmp(token, "}"));
  assert(current_material != nullptr);
  return new Sphere(center, radius, current_material);
}

Plane *SceneParser::parsePlane() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "normal"));
  Vector3f normal = readVector3f();
  getToken(token);
  assert(!strcmp(token, "offset"));
  float offset = readFloat();
  getToken(token);
  assert(!strcmp(token, "}"));
  assert(current_material != nullptr);
  return new Plane(normal, offset, current_material);
}

Disk *SceneParser::parseDisk() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "center"));
  Vector3f center = readVector3f();
  getToken(token);
  assert(!strcmp(token, "normal"));
  Vector3f normal = readVector3f();
  getToken(token);
  assert(!strcmp(token, "radius"));
  float radius = readFloat();
  getToken(token);
  assert(!strcmp(token, "}"));
  assert(current_material != nullptr);
  return new Disk(center, normal, radius, current_material);
}

Triangle *SceneParser::parseTriangle() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "vertex0"));
  Vector3f v0 = readVector3f();
  getToken(token);
  assert(!strcmp(token, "vertex1"));
  Vector3f v1 = readVector3f();
  getToken(token);
  assert(!strcmp(token, "vertex2"));
  Vector3f v2 = readVector3f();
  getToken(token);
  assert(!strcmp(token, "}"));
  assert(current_material != nullptr);
  return new Triangle(v0, v1, v2, current_material);
}

Mesh *SceneParser::parseTriangleMesh() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  char filename[MAX_PARSER_TOKEN_LENGTH];
  // get the filename
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  assert(!strcmp(token, "obj_file"));
  getToken(filename);
  getToken(token);
  assert(!strcmp(token, "}"));
  const char *ext = &filename[strlen(filename) - 4];
  assert(!strcmp(ext, ".obj"));
  Mesh *answer = new Mesh(filename, current_material);

  return answer;
}

Transform *SceneParser::parseTransform() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  Affine3f tr(Affine3f::Identity());
  Object3D *object = nullptr;
  getToken(token);
  assert(!strcmp(token, "{"));
  // read in transformations:
  // apply to the LEFT side of the current matrix (so the first
  // transform in the list is the last applied to the object)
  getToken(token);

  while (parseMatrix4f(token, tr)) getToken(token);
  // this must be an object,
  // and there are no more transformations
  object = parseObject(token);

  assert(object != nullptr);
  getToken(token);
  assert(!strcmp(token, "}"));
  return new Transform(tr, object);
}

MotionBlur *SceneParser::parseMotionBlur() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  Affine3f tr(Affine3f::Identity());
  Object3D *object = nullptr;
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  while (parseMatrix4f(token, tr)) getToken(token);
  object = parseObject(token);
  assert(object != nullptr);
  getToken(token);
  assert(!strcmp(token, "}"));
  return new MotionBlur(tr, object);
}

Fractal *SceneParser::parseFractal() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  getToken(token);
  assert(!strcmp(token, "{"));

  // read in the number of transforms
  getToken(token);
  assert(!strcmp(token, "numTransforms"));
  int num_transforms = readInt();
  vector<Affine3f> trs;

  // read in the transforms
  int count = 0;
  while (num_transforms > count) {
    Affine3f tr(Affine3f::Identity());
    getToken(token);
    assert(!strcmp(token, "Transform"));
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    while (parseMatrix4f(token, tr)) getToken(token);
    assert(!strcmp(token, "}"));
    trs.push_back(tr);
    count++;
  }

  // read in the object
  Object3D *object = nullptr;
  getToken(token);
  object = parseObject(token);
  assert(object != nullptr);

  getToken(token);
  assert(!strcmp(token, "}"));
  return new Fractal(trs, object);
}

Portal *SceneParser::parsePortal() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  Affine3f tr(Affine3f::Identity());
  Object3D *object = nullptr;
  getToken(token);
  assert(!strcmp(token, "{"));
  getToken(token);
  while (parseMatrix4f(token, tr)) getToken(token);
  object = parseObject(token);
  assert(object != nullptr);
  getToken(token);
  assert(!strcmp(token, "}"));
  return new Portal(tr, object);
}

// ====================================================================
// ====================================================================

int SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
  // for simplicity, tokens must be separated by whitespace
  assert(file != nullptr);
  int success = fscanf(file, "%s ", token);
  if (success == EOF) {
    token[0] = '\0';
    return 0;
  }
  return 1;
}

Vector3f SceneParser::readVector3f() {
  float x, y, z;
  int count = fscanf(file, "%f %f %f", &x, &y, &z);
  if (count != 3) {
    printf("Error trying to read 3 floats to make a Vector3f\n");
    assert(0);
  }
  return Vector3f(x, y, z);
}

bool SceneParser::parseMatrix4f(char token[MAX_PARSER_TOKEN_LENGTH],
                                Affine3f &tr) {
  if (!strcmp(token, "Scale")) {
    tr.scale(readVector3f());
  } else if (!strcmp(token, "UniformScale")) {
    tr.scale(readFloat());
  } else if (!strcmp(token, "Translate")) {
    tr.translate(readVector3f());
  } else if (!strcmp(token, "XRotate")) {
    tr.rotate(Eigen::AngleAxisf(Deg2Rad(readFloat()), Vector3f::UnitX()));
  } else if (!strcmp(token, "YRotate")) {
    tr.rotate(Eigen::AngleAxisf(Deg2Rad(readFloat()), Vector3f::UnitY()));
  } else if (!strcmp(token, "ZRotate")) {
    tr.rotate(Eigen::AngleAxisf(Deg2Rad(readFloat()), Vector3f::UnitZ()));
  } else if (!strcmp(token, "Rotate")) {
    getToken(token);
    assert(!strcmp(token, "{"));
    Vector3f axis = readVector3f().normalized();
    float angle = Deg2Rad(readFloat());
    tr.rotate(Eigen::AngleAxisf(angle, axis));
    getToken(token);
    assert(!strcmp(token, "}"));
  } else if (!strcmp(token, "Matrix4f")) {
    Matrix4f m;
    getToken(token);
    assert(!strcmp(token, "{"));
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        m(i, j) = readFloat();
      }
    }
    getToken(token);
    assert(!strcmp(token, "}"));
    tr *= m;
  } else {
    return false;
  }
  return true;
}

float SceneParser::readFloat() {
  float answer;
  int count = fscanf(file, "%f", &answer);
  if (count != 1) {
    printf("Error trying to read 1 float\n");
    assert(0);
  }
  return answer;
}

int SceneParser::readInt() {
  int answer;
  int count = fscanf(file, "%d", &answer);
  if (count != 1) {
    printf("Error trying to read 1 int\n");
    assert(0);
  }
  return answer;
}
