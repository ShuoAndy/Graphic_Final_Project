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
            int len = end - start;
            if (len == 1) {
                left_node = right_node = objects[start];
            } else if (len == 2) {
                left_node = objects[start];
                right_node = objects[start + 1];
            } else {
                auto temp_objects = objects;
                if (axis == 0){
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end, CompareXAxis);
                } else if (axis == 1) {
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end, CompareYAxis);
                } else {
                    std::sort(temp_objects.begin() + start, temp_objects.begin() + end, CompareZAxis);
                }
                int mid = start + len / 2;
                left_node = new KDTreeNode(temp_objects, start, mid, depth + 1);
                right_node = new KDTreeNode(temp_objects, mid, end, depth + 1);
            }
            Box box1, box2;
            left_node->getBox(box1);
            right_node->getBox(box2);
            box = mergeBox(box1, box2);
        }

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {
            bool h_left = false, h_right = false;
            Box box_left, box_right;
            left_node->getBox(box_left);
            right_node->getBox(box_right);
            if (box_left.intersect(r, -0x3f3f3f3f, 0x3f3f3f3f))
                h_left = left_node->intersect(r, h, tmin);
            if (box_right.intersect(r, -0x3f3f3f3f, 0x3f3f3f3f))
                h_right = right_node->intersect(r, h, tmin);
            return h_left || h_right;
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
        Box box;

};

#endif