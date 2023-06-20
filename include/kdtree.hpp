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

//实现kdtree树【数据结构课上讲过】，原理详见https://www.cnblogs.com/KillerAery/p/10878367.html#%E5%B1%82%E6%AC%A1%E5%8C%85%E5%9B%B4%E7%9B%92%E6%A0%91-bounding-volume-hierarchy-based-on-tree
//即求交加速的包围盒与层次结构

class KDTreeNode: public Object3D {
    public:
        KDTreeNode() {
            left_node = right_node = nullptr;
        }

        KDTreeNode(std::vector<Object3D*> &objects, int start, int end, int depth) {
            int xyz = depth % 3;
            left_node = right_node = nullptr;

            if (start == end)
            {
                objects[start]->getBox(box);
                obj = objects[start];
                return;
            }

            if (end == (start + 1)) 
            {
                objects[start]->getBox(box);
                obj = objects[start];
                left_node = objects[end];
            } 
            
            else if (end == (start + 2)) 
            {
                objects[start]->getBox(box);
                obj = objects[start];
                left_node = objects[start + 1];
                right_node = objects[end];
            } 
            
            else 
            {
                auto new_objects = objects;
                int mid = (start + end) / 2 ;
                if (xyz == 0)//在x维度划分
                {
                    std::sort(new_objects.begin() + start, new_objects.begin() + end + 1, sort_by_x);
                } 
                
                else if (xyz == 1)//在y维度划分
                {
                    std::sort(new_objects.begin() + start, new_objects.begin() + end + 1, sort_by_y);
                } 
                
                else //在z维度划分
                {
                    std::sort(new_objects.begin() + start, new_objects.begin() + end + 1, sort_by_z);
                }

                obj = new_objects[mid];
                obj->getBox(box);
                
                left_node = new KDTreeNode(new_objects, start, mid - 1, depth + 1);
                right_node = new KDTreeNode(new_objects, mid + 1, end, depth + 1);            
            }

            Box box1, box2;
            if (left_node) {left_node->getBox(box1); box = mergeBox(box, box1);}
            if (right_node) {right_node->getBox(box2); box = mergeBox(box, box2);};
        
        }

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {

            if (!box.intersect(r, -0x3f3f3f3f, 0x3f3f3f3f, tmin)) 
                return false;

            bool now_hit = obj->intersect(r, h, tmin);
            bool left_hit = false;
            bool right_hit = false;
            if (left_node)
                left_hit = left_node->intersect(r, h, tmin);
            if (right_node)
                right_hit = right_node->intersect(r, h, tmin);
            
            return now_hit || left_hit || right_hit;
        }

        virtual bool getBox(Box& box) override {
            box = this->box;
            return true;
        }

        static bool sort_by_x(Object3D* a, Object3D* b) {
            Box aBox, bBox;
            if (!a->getBox(aBox) || !b->getBox(bBox))
                return false;
            return aBox.min().x() < bBox.min().x();
        }

        static bool sort_by_y(Object3D* a, Object3D* b) {
            Box aBox, bBox;
            if (!a->getBox(aBox) || !b->getBox(bBox))
                return false;
            return aBox.min().y() < bBox.min().y();
        }

        static bool sort_by_z(Object3D* a, Object3D* b) {
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
        int xyz;

};

#endif