#ifndef KDTREE_H
#define KDTREE_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "box.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>

class KDTreeNode: public Object3D {
    public:
        KDTreeNode() {
            left_node = right_node = nullptr;
        }

        KDTreeNode(std::vector<Object3D*> &objects, int start, int end, int depth) {
            int axis = depth % 3;
            left_node = right_node = nullptr;

            if (start == end){
                objects[start]->getBox(box);
                obj = objects[start];
                return;
            }

            if (end == start + 1) {

                objects[start]->getBox(box);
                obj = objects[start];
                left_node = objects[end];

            } else if (end == start + 2) {

                objects[start]->getBox(box);
                obj = objects[start];
                left_node = objects[start + 1];
                right_node = objects[end];

            } else {

                auto temp_objects = objects;
                int mid = (start + end) / 2 ;
                if (axis == 0){
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end + 1, CompareXAxis);
                } else if (axis == 1) {
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end + 1, CompareYAxis);
                } else {
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end + 1, CompareZAxis);
                }
                obj = temp_objects[mid];
                obj->getBox(box);
                
                if (start < mid) 
                    left_node = new KDTreeNode(temp_objects, start, mid - 1, depth + 1);
                if (end > mid)
                    right_node = new KDTreeNode(temp_objects, mid + 1, end, depth + 1);
                
            }

            Box box1, box2;
            if (left_node) {left_node->getBox(box1); box = mergeBox(box, box1);}
            if (right_node) {right_node->getBox(box2); box = mergeBox(box, box2);};
        
        }

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {
            if (!box.intersect(r, -0x3f3f3f3f, 0x3f3f3f3f, tmin)) return false;
            bool has_hit = obj->intersect(r, h, tmin);
            bool h_left = false, h_right = false;
            if (left_node)
                h_left = left_node->intersect(r, h, tmin);
            if (right_node)
                h_right = right_node->intersect(r, h, tmin);
            return h_left || h_right || has_hit;
        }

        virtual bool getBox(Box& box) override {
            box = this->box;
            return true;
        }

        // Compare two boxes from three dimensions
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
        Object3D* obj;
        Box box;
        int axis;
        int axis_bound;

};

#endif