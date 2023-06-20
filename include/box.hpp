#ifndef BOX_H
#define BOX_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include "object3d.hpp"
#include <Vector3f.h>
#include <cmath>
#include <vector>

using namespace std;

//实现AABB包围盒，教程详见https://blog.csdn.net/weixin_43022263/article/details/108550538

class Box {
    public:
        Box() {}
        Box(const Vector3f& a, const Vector3f& b){
            min_point = a;
            max_point = b;
        }

        bool intersect(const Ray& r, float t_min, float t_max, float tmin = -0x3f3f3f3f) {
            
            //在xyz三个维度上计算相交
            
            float dir=r.getDirection().x();
            if(dir==0.0f) return true;
            float t0 = (min_point.x() - r.getOrigin().x()) /dir;
            float t1 = (max_point.x() - r.getOrigin().x()) /dir;
            if (dir < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) 
                return false;

            dir=r.getDirection().y();
            if(dir==0) return true;
            t0 = (min_point.y() - r.getOrigin().y()) /dir;
            t1 = (max_point.y() - r.getOrigin().y()) /dir;
            if (dir < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) 
                return false;
            
            dir=r.getDirection().z();
            if(dir==0) return true;
            t0 = (min_point.z() - r.getOrigin().z()) /dir;
            t1 = (max_point.z() - r.getOrigin().z()) /dir;
            if (dir < 0.0f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) 
                return false;

            return true;
        }

        Vector3f min() {
            return min_point;
        }
        Vector3f max() {
            return max_point;
        }

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