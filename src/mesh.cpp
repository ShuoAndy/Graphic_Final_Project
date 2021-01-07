#include "mesh.hpp"
#include "box.hpp"
#include "bvh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <vector>

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {
    bool flag = false;
    if (strcmp(accelerator, "bvh") == 0){
        flag = BVHRoot->intersect(r, h, tmin);
    } else if (strcmp(accelerator, "kdtree") == 0) {
        flag = KDTreeRoot->intersect(r, h, tmin);
    } else {
        for (auto obj: triangle_list) {
            if (obj->intersect(r, h, tmin))
                flag = true;
        }
    }
    return flag;
}

bool Mesh::getBox(Box &box) {
    box = this->box;
    return true;
}

void Mesh::buildTree() {
    std::cout << "start building tree for mesh " << std::endl;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle* triangle_ptr = new Triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle_ptr->normal = n[triId];
        triangle_list.push_back(triangle_ptr);
    }
    if (strcmp(accelerator, "bvh") == 0){
        std::cout << "start building BVH accelerator for mesh " << std::endl;
        BVHRoot = new BVHNode(triangle_list, 0, t.size());
    }
    else if (strcmp(accelerator, "kdtree") == 0){
        std::cout << "start building KDTree accelerator for mesh " << std::endl;
        KDTreeRoot = new KDTreeNode(triangle_list, 0, t.size(), 0);
    }
    else {
        fprintf(stderr, "unsupported accelerator %s, using default intersection method\n", accelerator);
    }
}

Mesh::Mesh(const char *filename, Material *material, const char* accelerator) : Object3D(material) {

    this->accelerator = accelerator;
    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
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
    buildTree();
    Box temp;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle.getBox(temp);
        if (triId == 0) {
            box = temp;
        } else {
            box = mergeBox(box, temp);
        }
    }
    f.close();
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}
