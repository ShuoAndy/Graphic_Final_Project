#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->normal = normal;
        this->d = d;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f r_dir = r.getDirection();
        if(abs(Vector3f::dot(r_dir.normalized(), this->normal.normalized())) < 1e-10)
            return false;
        float t = (this->d - Vector3f::dot(this->normal, r.getOrigin()))/(Vector3f::dot(this->normal, r.getDirection()));
        if (t < tmin || t > h.getT())
            return false;
        if (Vector3f::dot(r.getDirection(), this->normal) > 0)
            h.set(t, this->material, -this->normal.normalized());
        else
            h.set(t, this->material, this->normal.normalized());
        return true;
    }

protected:
    Vector3f normal;
    float d;
};

#endif //PLANE_H
		

