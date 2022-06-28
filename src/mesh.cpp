#include "mesh.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Mesh::Mesh(const char *filename, Material *material) {
  vector<Vector3f> v, vn;
  vector<Vector2f> vt;
  vector<array<TriangleIndex, 3>> f;
  vector<Triangle *> triangles;

  // read obj file
  const string vTok("v"), vtTok("vt"), vnTok("vn"), fTok("f");
  ifstream fs;
  fs.open(filename);
  if (!fs.is_open()) {
    cout << "Cannot open " << filename << "\n";
    return;
  }
  string line, tok;
  int texID;
  while (true) {
    getline(fs, line);
    if (fs.eof()) break;
    if (line.size() < 3) continue;
    if (line.at(0) == '#') continue;
    stringstream ss(line);
    ss >> tok;
    if (tok == vTok) {
      Vector3f vec;
      ss >> vec[0] >> vec[1] >> vec[2];
      v.push_back(vec);
    } else if (tok == vtTok) {
      Vector2f texcoord;
      ss >> texcoord[0];
      ss >> texcoord[1];
      vt.push_back(texcoord);
    } else if (tok == vnTok) {
      Vector3f vec;
      ss >> vec[0] >> vec[1] >> vec[2];
      vn.push_back(vec);
    } else if (tok == fTok) {
      array<TriangleIndex, 3> vtn;
      for (int i = 0; i < 3; ++i) {
        string raw, parsed;
        ss >> raw;
        stringstream vtns(raw);
        for (TriangleIndex &idx : vtn) {
          idx[i] = -1;
          if (!vtns.eof()) {
            getline(vtns, parsed, '/');
            idx[i] += atoi(parsed.c_str());
          }
        }
      }
      f.push_back(vtn);
    }
  }
  fs.close();

  // construct triangles
  for (const auto &vtn : f) {
    auto v_idx = vtn[0], t_idx = vtn[1], n_idx = vtn[2];
    auto trig = new Triangle(v[v_idx[0]], v[v_idx[1]], v[v_idx[2]], material);
    if (t_idx.valid()) trig->setVt(vt[t_idx[0]], vt[t_idx[1]], vt[t_idx[2]]);
    if (n_idx.valid()) trig->setVn(vn[n_idx[0]], vn[n_idx[1]], vn[n_idx[2]]);
    triangles.push_back(trig);
  }

  // build kd tree
  *this = Mesh(triangles.begin(), triangles.end());
}

Mesh::Mesh(vector<Triangle *>::iterator begin,
           vector<Triangle *>::iterator end) {
  size_t n = end - begin;
  if (n < 4) {
    for (auto it = begin; it < end; ++it) addObject(*it);
    return;
  }
  int axis = -1;
  bbox.sizes().maxCoeff(&axis);
  sort(begin, end, [&](Triangle *a, Triangle *b) {
    return a->getBBox().center()[axis] < b->getBBox().center()[axis];
  });
  addObject(new Mesh(begin + n / 2, end));
  addObject(new Mesh(begin, begin + n / 2));
}

bool Mesh::intersect(const Ray &r, Hit &h, Object3D *&obj, float tmin) {
  return Group::intersect(r, h, obj, tmin);
}
