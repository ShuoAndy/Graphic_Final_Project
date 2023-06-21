#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "box.hpp"
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

    explicit Group (int num_objects, const char* accelerator):list(num_objects) {
        this->accelerator = accelerator;
    }

    ~Group() override {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool result = false;
        if (strcmp(accelerator, "kdtree") == 0) {
            for (auto obj: infinities){
                if (obj->intersect(r, h, tmin)) result = true;
            }
            
            result |= KDTreeRoot->intersect(r, h, tmin);
        } else {
            for (auto obj : list)
            if (obj) result |= obj->intersect(r, h, tmin);
        }
        return result;
    }

    bool getBox(Box &box) override {
        bool result = true;
        Box temp;
        for (auto obj: list) {
            if (!obj->getBox(temp)) return false;
            if (result) {
                box = temp;
            } else {
                result = false;
                box = mergeBox(box, temp);
            }
        }
        return true;
    }

    void addObject(int index, Object3D *obj) {
        this->list[index] = obj;
    }

    void buildTree() {
        std::cout << "start building accelerator for group" << std::endl;
        std::vector<Object3D*> finite;
        for (auto obj: this->list) {
            if (obj->is_infinite) this->infinities.push_back(obj);
            else finite.push_back(obj);
        }
        std::cout << "finite objects: " << finite.size() <<  ", infinite objects: " << infinities.size() << std::endl; 
        if (strcmp(accelerator, "kdtree") == 0) {
            KDTreeRoot = new KDTreeNode(finite, 0, finite.size() - 1, 0);
        }

    }

    std::vector<Object3D*> getLightSources() {
        std::vector<Object3D*> light_sources;
        for (auto obj: list){
            if (obj->getMaterial()->getSpecularColor() != Vector3f::ZERO){
                light_sources.push_back(obj);
            }
        }
        return light_sources;
    }

    int getGroupSize() {
        return list.size();
    }

private:
    std::vector<Object3D*> list;
    std::vector<Object3D*> infinities;
    KDTreeNode* KDTreeRoot;
    const char* accelerator;

};

#endif
	
