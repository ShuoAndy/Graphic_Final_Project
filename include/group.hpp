#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "box.hpp"
#include "bvh.hpp"
#include "kdtree.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects, const char* accelerator) {
        this->objects.resize(num_objects, nullptr);
        this->group_size = num_objects;
        this->accelerator = accelerator;
    }

    ~Group() override {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool flag = false;
        if (strcmp(accelerator, "bvh") == 0){
            for (auto obj: infinities){
                if (obj->intersect(r, h, tmin)) flag = true;
            }
            flag |= BVHRoot->intersect(r, h, tmin);
        } else if (strcmp(accelerator, "kdtree") == 0) {
            for (auto obj: infinities){
                if (obj->intersect(r, h, tmin)) flag = true;
            }
            flag |= KDTreeRoot->intersect(r, h, tmin);
        } else {
            for (auto obj: objects) {
                if (obj->intersect(r, h, tmin))
                    flag = true;
            }
        }
        return flag;
    }

    bool getBox(Box &box) override {
        bool flag = true;
        Box temp;
        for (auto obj: objects) {
            if (!obj->getBox(temp)) return false;
            if (flag) {
                box = temp;
            } else {
                flag = false;
                box = mergeBox(box, temp);
            }
        }
        return true;
    }

    void addObject(int index, Object3D *obj) {
        this->objects[index] = obj;
    }

    void buildTree() {
        std::cout << "start building accelerator for group" << std::endl;
        std::vector<Object3D*> finite;
        for (auto obj: this->objects) {
            if (obj->is_infinite) this->infinities.push_back(obj);
            else finite.push_back(obj);
        }
        std::cout << "finite objects: " << finite.size() <<  ", infinite objects: " << infinities.size() << std::endl; 
        if (strcmp(accelerator, "bvh") == 0){
            BVHRoot = new BVHNode(finite, 0, finite.size());
        } else if (strcmp(accelerator, "kdtree") == 0) {
            KDTreeRoot = new KDTreeNode(finite, 0, finite.size(), 0);
        }
        else {
            fprintf(stderr, "unsupported accelerator %s using default intersection method\n", accelerator);
        }
    }

    std::vector<Object3D*> getLightSources() {
        std::vector<Object3D*> light_sources;
        for (auto obj: objects){
            if (obj->getMaterial()->Emission() != Vector3f::ZERO){
                light_sources.push_back(obj);
            }
        }
        return light_sources;
    }

    int getGroupSize() {
        return this->group_size;
    }

private:
    std::vector<Object3D*> objects;
    std::vector<Object3D*> infinities;
    int group_size;
    BVHNode* BVHRoot;
    KDTreeNode* KDTreeRoot;
    const char* accelerator;

};

#endif
	
