#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include "bvh.hpp"
#include "kdtree.hpp"


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m, const char* accelerator);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    bool intersect(const Ray &r, Hit &h, float tmin) override;
    virtual bool getBox(Box& box) override;
    void buildTree();

private:

    // Normal can be used for light estimation
    void computeNormal();
    BVHNode* BVHRoot;
    KDTreeNode* KDTreeRoot;
    std::vector<Object3D*> triangle_list;
    const char* accelerator;
};

#endif
