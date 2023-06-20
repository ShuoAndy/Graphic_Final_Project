#ifndef BVH_H
#define BVH_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "box.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>

//实现kdtree树【数据结构课上讲过】，原理详见https://www.cnblogs.com/KillerAery/p/10878367.html#%E5%B1%82%E6%AC%A1%E5%8C%85%E5%9B%B4%E7%9B%92%E6%A0%91-bounding-volume-hierarchy-based-on-tree
class BVHNode: public Object3D {
    public:
        BVHNode() {
            left_node = right_node = nullptr;
        }

        BVHNode(std::vector<Object3D*> &objects, int start, int end) {
            int axis = int(3*drand48()); //随机选取x或y或z
            int len = end - start;
            if (len == 1) //只有左孩子
            {
                left_node = right_node = objects[start];
            } 
            else if (len == 2) //有左右孩子
            {
                left_node = objects[start];
                right_node = objects[start + 1];
            } 
            else 
            {
                auto temp_objects = objects;
                if (axis == 0){
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end, CompareXAxis);
                } else if (axis == 1) {
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end, CompareYAxis);
                } else {
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end, CompareZAxis);
                }
                int mid = start + len / 2;
                left_node = new BVHNode(temp_objects, start, mid);
                right_node = new BVHNode(temp_objects, mid, end);
            }
            Box box1, box2;
            left_node->getBox(box1);
            right_node->getBox(box2);
            box = mergeBox(box1, box2);
        }

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {
            if (!box.intersect(r, -0x3f3f3f3f, 0x3f3f3f3f, tmin)){
                return false;
            }
            bool h_left = left_node->intersect(r, h, tmin);
            bool h_right = right_node->intersect(r, h, tmin);
            return h_left || h_right;
        }

        virtual bool getBox(Box& box) override {
            box = this->box;
            return true;
        }

        static bool CompareXAxis(Object3D* a, Object3D* b) {
            Box aBox, bBox;
            if (!a->getBox(aBox) || !b->getBox(bBox))
                return false;
            return aBox.min().x() < bBox.min().x();
        }

        static bool CompareYAxis(Object3D* a, Object3D* b) {
            Box aBox, bBox;
            if (!a->getBox(aBox) || !b->getBox(bBox))
                return false;
            return aBox.min().y() < bBox.min().y();
        }

        static bool CompareZAxis(Object3D* a, Object3D* b) {
            Box aBox, bBox;
            if (!a->getBox(aBox) || !b->getBox(bBox))
                return false;
            return aBox.min().z() < bBox.min().z();
        }

    private:
        Object3D* left_node;
        Object3D* right_node;
        Box box;
};


#endif