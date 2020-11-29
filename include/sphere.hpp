#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        // 
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        //
        Vector3f sphere_dir = this->center - r.getOrigin();
        float origin_distance = sphere_dir.length();
        float t = origin_distance*(Vector3f::dot(r.getDirection().normalized(), sphere_dir.normalized()));
        if (powf(origin_distance, 2) - powf(t, 2) > powf(this->radius, 2))
            return false;
        float inter_t = t - sqrt(powf(this->radius, 2) - powf(origin_distance, 2) + powf(t, 2));
        if (inter_t < tmin || inter_t > h.getT())
            return false;
        Vector3f inter_dir;
        if (this->radius > 0)
            inter_dir =  r.pointAtParameter(inter_t) - this->center;
        else
            inter_dir =  this->center - r.pointAtParameter(inter_t);
            
        h.set(inter_t, this->material, inter_dir.normalized());
        return true;
    }

protected:
    Vector3f center;
    float radius;

};


#endif
