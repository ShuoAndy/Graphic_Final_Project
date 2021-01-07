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
    if (strcmp(accelerator, "bvh") == 0){
        std::cout << "start building BVH accelerator for mesh " << std::endl;
        BVHRoot = new BVHNode(triangle_list, 0, triangle_list.size());
    }
    else if (strcmp(accelerator, "kdtree") == 0){
        std::cout << "start building KDTree accelerator for mesh " << std::endl;
        KDTreeRoot = new KDTreeNode(triangle_list, 0, triangle_list.size(), 0);
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
    std::vector<TriangleIndex> vIdx, tIdx, nIdx;
    std::vector<Vector3f> v, vn;
    std::vector<Vector2f> vt;
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string vnTok("vn");
    std::string texTok("vt");
    std::string bslash("/"), space(" ");
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

            bool tFlag = 1, nFlag = 1;
            TriangleIndex vId, tId, nId;
            for (int i = 0; i < 3; ++i) {
                std::string str;
                ss >> str;

                std::vector<std::string> id;
                std::string::size_type pos;
                std::vector<std::string> result;
                str += bslash;
                int size = str.size();

                for (int i = 0; i < size; i++) {
                    pos = str.find(bslash, i);
                    if (pos < size) {
                        std::string s = str.substr(i, pos - i);
                        id.push_back(s);
                        i = pos + bslash.size() - 1;
                    }
                }

                vId[i] = atoi(id[0].c_str()) - 1;
                if (id.size() > 1) {
                    tId[i] = atoi(id[1].c_str()) - 1;
                }
                if (id.size() > 2) {
                    nId[i] = atoi(id[2].c_str()) - 1;
                }
            }
            vIdx.push_back(vId);
            tIdx.push_back(tId);
            nIdx.push_back(nId);

        } else if (tok == texTok) {

            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            vt.push_back(texcoord);

        } else if (tok == vnTok) {

            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vn.push_back(vec);
        }
    }
    f.close();
    Box temp;
    for (int triId = 0; triId < (int)vIdx.size(); ++triId) {
        TriangleIndex &vIndex = vIdx[triId];
        triangle_list.push_back((Object3D *)new Triangle(
            v[vIndex[0]], v[vIndex[1]], v[vIndex[2]], material));
        ((Triangle *)triangle_list.back())->getBox(temp);
        triId == 0 ? (box = temp) : (box = mergeBox(box, temp));
        TriangleIndex &tIndex = tIdx[triId];
        if (tIndex.valid()) ((Triangle *)triangle_list.back())->setTexCoord(vt[tIndex[0]], vt[tIndex[1]], vt[tIndex[2]]);
        TriangleIndex &nIndex = nIdx[triId];
        if (nIndex.valid()) ((Triangle *)triangle_list.back())->setSmoothedNorm(vn[nIndex[0]], vn[nIndex[1]], vn[nIndex[2]]);
    }
    buildTree();

}

