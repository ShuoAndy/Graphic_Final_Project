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
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f r_dir = r.getDirection();
        if(abs(Vector3f::dot(r_dir.normalized(), this->normal.normalized())) < 1e-10)
            return false;
        float t = (this->d - Vector3f::dot(this->normal, r.getOrigin()))/(Vector3f::dot(this->normal, r.getDirection()));
        if (t < tmin || t > h.getT())
            return false;

        Vector3f hit_point = r.pointAtParameter(t);
		float v = Vector3f::dot(hit_point, vdim);
        float u = Vector3f::dot(hit_point, udim);
        Vector3f norm = getNorm(u, v, hit_point);
        h.set(t, u, v, this->material, norm.normalized(), r);
        return true;
    }

    bool getBox(Box& box) override {
        Vector3f norm = normal.normalized();
        Vector3f max_point = float(10000) * Vector3f(norm.x() < 1 - 1e-6, norm.y() < 1 - 1e-6, norm.z() < 1 - 1e-6) + d * norm;
        Vector3f min_point =  -float(10000) * Vector3f(norm.x() < 1 - 1e-6, norm.y() < 1 - 1e-6, norm.z() < 1 - 1e-6) + d * norm;
        box = Box(min_point, max_point);
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
        float value = 0;
        Vector2f grad = material->getBump()->GradAt(u, v, value);
        return Vector3f::cross(udim + grad[0] * normal.normalized(), vdim + grad[1] * normal.normalized());
    }
};

#endif //PLANE_H
		

