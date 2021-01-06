#ifndef BOX_H
#define BOX_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include <Vector3f.h>

class Box {
    public:
        Box() {}
        Box(const Vector3f& a, const Vector3f& b){
            min_point = a;
            max_point = b;
        }

        bool intersect(const Ray& r, float t_min, float t_max) {
            float inv = 1.0f / r.getDirection().x();
            float t0 = (min_point.x() - r.getOrigin().x()) * inv;
            float t1 = (max_point.x() - r.getOrigin().x()) * inv;
            if (inv < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) 
                return false;

            inv = 1.0f / r.getDirection().y();
            t0 = (min_point.y() - r.getOrigin().y()) * inv;
            t1 = (max_point.y() - r.getOrigin().y()) * inv;
            if (inv < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) 
                return false;
            
            inv = 1.0f / r.getDirection().z();
            t0 = (min_point.z() - r.getOrigin().z()) * inv;
            t1 = (max_point.z() - r.getOrigin().z()) * inv;
            if (inv < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) 
                return false;
            
            return true;
        }

        Vector3f min() {
            return min_point;
        }
        Vector3f max() {
            return max_point;
        }

        // Merge the two bounding boxes into one
        friend Box mergeBox(Box a, Box b){
            Vector3f min_point(fmin(a.min().x(), b.min().x()), fmin(a.min().y(), b.min().y()), fmin(a.min().z(), b.min().z()));
            Vector3f max_point(fmax(a.max().x(), b.max().x()), fmax(a.max().y(), b.max().y()), fmax(a.max().z(), b.max().z()));
            return Box(min_point, max_point);
        }

    private:
        Vector3f min_point;
        Vector3f max_point;
        
};

#endif