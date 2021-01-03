#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "box.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        this->objects.resize(num_objects, nullptr);
        this->group_size = num_objects;
    }

    ~Group() override {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool flag = false;
        for (auto obj: this->objects){
            if(obj->intersect(r, h, tmin))
                flag = true;
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

    int getGroupSize() {
        return this->group_size;
    }

private:
    std::vector<Object3D*> objects;
    int group_size;
};

#endif
	
