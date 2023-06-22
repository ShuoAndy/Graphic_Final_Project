#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <cassert>
#include <vecmath.h>
#include <cmath>
#include <limits>

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
        if (Vector3f::cross(this->normal.normalized(), Vector3f::UP).length() < 1e-6)
            this->vdim = Vector3f::FORWARD;
        else this->vdim = Vector3f::UP;
        this->udim = Vector3f::cross(this->normal.normalized(), this->vdim).normalized();
        is_infinite = true;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        //已完成
        if (fabs( Vector3f::dot(normal, r.getDirection())) < 1e-6) return false;
        float t = (d - Vector3f::dot(normal, r.getOrigin())) / Vector3f::dot(normal, r.getDirection());//参照ppt第16页

        Vector3f hit_point = r.pointAtParameter(t);
		float v = Vector3f::dot(hit_point, vdim);
        float u = Vector3f::dot(hit_point, udim);

        if( t>0 && t < h.getT() && t > tmin)
        {
            if(Vector3f::dot(normal, r.getDirection())<0)
                h.set(t, u, v, this->material, normal, r);
            else
                h.set(t, u, v, this->material, -normal, r);
            return true;
        }
        else
            return false;
    }

    bool getBox(Box& box) override {
        return true;
    }

protected:
    Vector3f normal;
    float d;
    Vector3f udim, vdim;
    Vector3f getNorm(float u, float v, Vector3f &hit_point) {
        if (!material->getBump()->hasBump()) {
            return normal;
        }
        Vector2f bump = material->getBump()->BumpAt(u, v);
        return Vector3f::cross(udim + bump[0] * normal.normalized(), vdim + bump[1] * normal.normalized());
    }
};

#endif //PLANE_H
		

