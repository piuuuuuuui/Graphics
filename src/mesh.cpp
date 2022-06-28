#include "mesh.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Mesh::Mesh(const string &filename, Material *material) {
  vector<Vector3d> v, vn;
  vector<Vector2d> vt;
  vector<Triangle *> triangles;
  map<string, Material *> materials;

  // read obj file
  ifstream fs;
  fs.open(filename);
  if (!fs.is_open()) {
    cout << "Cannot open " << filename << "\n";
    return;
  }
  string dir;
  const size_t last_slash_idx = filename.rfind('/');
  if (std::string::npos != last_slash_idx)
    dir = filename.substr(0, last_slash_idx);
  string line, tok;
  int texID;
  while (true) {
    getline(fs, line);
    if (fs.eof()) break;
    if (line.size() < 3) continue;
    if (line.at(0) == '#') continue;
    stringstream ss(line);
    ss >> tok;
    if (tok == "mtllib") {
      string mtlfile;
      ss >> mtlfile;  // no whitespace
      materials = parseMtl(dir, mtlfile);
    } else if (tok == "usemtl") {
      string mtlname;
      ss >> mtlname;
      material = materials[mtlname];
    } else if (tok == "v") {
      Vector3d vec;
      ss >> vec[0] >> vec[1] >> vec[2];
      v.push_back(vec);
    } else if (tok == "vt") {
      Vector2d vec;
      ss >> vec[0] >> vec[1];
      vt.push_back(vec);
    } else if (tok == "vn") {
      Vector3d vec;
      ss >> vec[0] >> vec[1] >> vec[2];
      vn.push_back(vec);
    } else if (tok == "f") {
      array<TriangleIndex, 3> vtn;  // v, vt, vn
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
      auto v_idx = vtn[0], t_idx = vtn[1], n_idx = vtn[2];
      auto trig = new Triangle(v[v_idx[0]], v[v_idx[1]], v[v_idx[2]], material);
      if (t_idx.valid()) trig->setVt(vt[t_idx[0]], vt[t_idx[1]], vt[t_idx[2]]);
      if (n_idx.valid()) trig->setVn(vn[n_idx[0]], vn[n_idx[1]], vn[n_idx[2]]);
      triangles.push_back(trig);
    }
  }
  fs.close();

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

bool Mesh::intersect(const Ray &r, Hit &h, Object3D *&obj, double tmin) {
  if (!Group::intersect(r, h, obj, tmin)) return false;
  obj = this;
  return true;
}

map<string, Material *> Mesh::parseMtl(const string &dir,
                                       const string &filename) {
  map<string, Material *> materials;
  Material *current_material = nullptr;

  ifstream fs;
  fs.open(dir + '/' + filename);
  if (!fs.is_open()) {
    cout << "Cannot open " << dir + '/' + filename << "\n";
    return materials;
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
    if (tok == "newmtl") {
      string mtlname;
      ss >> mtlname;  // no whitespace
      current_material = new Material;
      materials[mtlname] = current_material;
    } else if (tok == "Ke") {
      Vector3d &vec = current_material->emissiveColor;
      ss >> vec[0] >> vec[1] >> vec[2];
    } else if (tok == "Kd") {
      Vector3d &vec = current_material->diffuseColor;
      ss >> vec[0] >> vec[1] >> vec[2];
    } else if (tok == "Ks") {
      Vector3d &vec = current_material->specularColor;
      ss >> vec[0] >> vec[1] >> vec[2];
    } else if (tok == "Ns") {  // Phong model
      double Ns;
      ss >> Ns;
      current_material->roughness = exp(-1e-2 * Ns);
    } else if (tok == "d") {
      ss >> current_material->opacity;
    } else if (tok == "Tr") {
      double transparency;
      ss >> transparency;
      current_material->opacity = 1 - transparency;
    } else if (tok == "Tf") {
      Vector3d &vec = current_material->refractiveColor;
      ss >> vec[0] >> vec[1] >> vec[2];
    } else if (tok == "Ni") {
      ss >> current_material->refractiveIndex;
    } else if (tok == "map_Ke") {
      string texfile;
      ss >> texfile;
      current_material->emissiveMap = new Texture(dir + '/' + texfile);
    } else if (tok == "map_Kd") {
      string texfile;
      ss >> texfile;
      current_material->diffuseMap = new Texture(dir + '/' + texfile);
    } else if (tok == "map_Ks") {
      string texfile;
      ss >> texfile;
      current_material->specularMap = new Texture(dir + '/' + texfile);
    } else if (tok == "map_d") {
      string texfile;
      ss >> texfile;
      current_material->opacityMap = new Texture(dir + '/' + texfile);
    } else if (tok == "map_Bump" || tok == "map_bump" || tok == "bump") {
      string texfile;
      ss >> texfile;
      current_material->bumpMap = new Texture(dir + '/' + texfile);
    }
  }
  fs.close();

  return materials;
}
