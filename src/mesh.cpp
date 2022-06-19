#include "mesh.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

bool Mesh::intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
  // Optional: Change this brute force method into a faster one.
  bool result = false;
  for (int triId = 0; triId < (int)t.size(); ++triId) {
    TriangleIndex &triIndex = t[triId];
    Triangle triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], material);
    triangle.normal = n[triId];
    result |= triangle.intersect(r, h, obj, tmin);
  }
  if (!result) return false;
  obj = this;
  return true;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {
  // Optional: Use tiny obj loader to replace this simple one.
  ifstream f;
  f.open(filename);
  if (!f.is_open()) {
    cout << "Cannot open " << filename << "\n";
    return;
  }
  string line;
  string vTok("v");
  string fTok("f");
  string texTok("vt");
  char bslash = '/', space = ' ';
  string tok;
  int texID;
  while (true) {
    getline(f, line);
    if (f.eof()) {
      break;
    }
    if (line.size() < 3) {
      continue;
    }
    if (line.at(0) == '#') {
      continue;
    }
    stringstream ss(line);
    ss >> tok;
    if (tok == vTok) {
      Vector3f vec;
      ss >> vec[0] >> vec[1] >> vec[2];
      v.push_back(vec);
    } else if (tok == fTok) {
      if (line.find(bslash) != string::npos) {
        replace(line.begin(), line.end(), bslash, space);
        stringstream facess(line);
        TriangleIndex trig;
        facess >> tok;
        for (int ii = 0; ii < 3; ii++) {
          facess >> trig[ii] >> texID;
          trig[ii]--;
        }
        t.push_back(trig);
      } else {
        TriangleIndex trig;
        for (int ii = 0; ii < 3; ii++) {
          ss >> trig[ii];
          trig[ii]--;
        }
        t.push_back(trig);
      }
    } else if (tok == texTok) {
      Vector2f texcoord;
      ss >> texcoord[0];
      ss >> texcoord[1];
    }
  }
  computeNormal();

  f.close();
}

void Mesh::computeNormal() {
  n.resize(t.size());
  for (int triId = 0; triId < (int)t.size(); ++triId) {
    TriangleIndex &triIndex = t[triId];
    Vector3f a = v[triIndex[1]] - v[triIndex[0]];
    Vector3f b = v[triIndex[2]] - v[triIndex[0]];
    b = a.cross(b);
    n[triId] = b / b.norm();
  }
}
