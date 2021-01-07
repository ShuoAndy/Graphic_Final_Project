#ifndef PHOTONKDTREE_H
#define PHOTONKDTREE_H

#include "hit.hpp"
#include "ray.hpp"
#include <cassert>
#include <iostream>
#include <Vector3f.h>
#include <vector>
#include <algorithm>
#include <cstdio>

using namespace std;

const float DiscountCoef = 0.7;

class PhotonKDTreeNode {
    public:
        PhotonKDTreeNode(){
            float inf = 0x3f3f3f3f;
            left_node = right_node = nullptr;
            hit = nullptr;
            max_radius = 0;
            min_point = Vector3f(inf);
            max_point = Vector3f(-1 * inf);
        }

        PhotonKDTreeNode(vector<Hit*> &hit_list, int start, int end, int depth) {
            int axis = depth % 3;
            float inf = 0x3f3f3f3f;
            min_point = Vector3f(inf);
            max_point = Vector3f(-1 * inf);
            left_node = right_node = nullptr;
            max_radius = 0;
            for (int i = start; i <= end; i ++) {
                min_point = Min(min_point, hit_list[i]->getPoint());
                max_point = Max(max_point, hit_list[i]->getPoint());
                max_radius = max(hit_list[i]->Radius, max_radius);
            }
            if (start == end){
                hit = hit_list[start];
                return;
            }

            int mid = (start + end) / 2 ;
            if ( axis == 0 ) {
                nth_element(hit_list.begin() + start, hit_list.begin() + mid, hit_list.begin() + end + 1, CompareXAxis);
            } else if (axis == 1) {
                nth_element(hit_list.begin() + start, hit_list.begin() + mid, hit_list.begin() + end + 1, CompareYAxis);
            } else {
                nth_element(hit_list.begin() + start, hit_list.begin() + mid, hit_list.begin() + end + 1, CompareZAxis);
            }
            hit = hit_list[mid];
            if (start < mid)
                left_node = new PhotonKDTreeNode(hit_list, start, mid - 1, depth + 1);
            if (mid < end)
                right_node = new PhotonKDTreeNode(hit_list, mid + 1, end, depth + 1);
        }

        void Update(const Vector3f& photon, const Vector3f& attenuation, bool front) {
            
            if (getRadius(photon) > max_radius) return;

            if ((hit->getPoint() - photon).squaredLength() <= hit->Radius && hit->isFrontFace() == front) {
                float disc = (hit->PhotonCount * DiscountCoef + DiscountCoef) / (hit->PhotonCount * DiscountCoef + 1.f);
                hit->PhotonCount ++;
                hit->Radius *= sqrt(disc);
                hit->PhotonFlux = (hit->PhotonFlux + hit->Attenuation * attenuation) * disc;
            }

            if (left_node) left_node->Update(photon, attenuation, front);
            if (right_node) right_node->Update(photon, attenuation, front);
            max_radius = hit->Radius;
            if (left_node) max_radius = max(left_node->max_radius, max_radius);
            if (right_node) max_radius = max(right_node->max_radius, max_radius);
            
        }

        // Compare two hits from three dimensions
        static bool CompareXAxis(Hit* a, Hit* b) {
            return a->getPoint().x() < b->getPoint().x();
        }

        static bool CompareYAxis(Hit* a, Hit* b) {
            return a->getPoint().y() < b->getPoint().y();
        }

        static bool CompareZAxis(Hit* a, Hit* b) {
            return a->getPoint().z() < b->getPoint().z();
        }
        PhotonKDTreeNode *left_node, *right_node;
    
    private:
        Hit* hit;
        float max_radius;
        Vector3f min_point, max_point;
        Vector3f Min(const Vector3f a, const Vector3f b) {
            return Vector3f(min(a.x(), b.x()), min(a.y(), b.y()), min(a.z(), b.z()));
        }

        Vector3f Max(const Vector3f a, const Vector3f b) {
            return Vector3f(max(a.x(), b.x()), max(a.y(), b.y()), max(a.z(), b.z()));
        }

        float getRadius(const Vector3f& photon) {
            float ret = 0;
            if (photon.x() > max_point.x()) ret += powf(photon.x() - max_point.x(), 2);
            if (photon.x() < min_point.x()) ret += powf(min_point.x() - photon.x(), 2);
            if (photon.y() > max_point.y()) ret += powf(photon.y() - max_point.y(), 2);
            if (photon.y() < min_point.y()) ret += powf(min_point.y() - photon.y(), 2);
            if (photon.z() > max_point.z()) ret += powf(photon.z() - max_point.z(), 2);
            if (photon.z() < min_point.z()) ret += powf(min_point.z() - photon.z(), 2);
            return ret;
        }
};
#endif