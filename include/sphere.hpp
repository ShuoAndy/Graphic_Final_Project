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
        float outer_t = t + sqrt(powf(this->radius, 2) - powf(origin_distance, 2) + powf(t, 2));
        float target_t;
        if (inter_t >= tmin && inter_t <= h.getT())
            target_t = inter_t;
        else if (outer_t >= tmin && outer_t <= h.getT())
            target_t = outer_t;
        else
            return false;
        Vector3f out_norm;
        out_norm =  r.pointAtParameter(target_t) - this->center;
        out_norm.normalize();
        float u = atan2(-out_norm.z(), out_norm.x()) / (2 * M_PI) + 0.5f, v = acos(-out_norm.y()) / M_PI;    
        h.set(target_t, u, v, this->material, getNorm(u, v, r.pointAtParameter(target_t) - this->center, out_norm), r);
        return true;
    }

    bool getBox(Box& box) override {
        box = Box(center - Vector3f(radius, radius, radius), center + Vector3f(radius, radius, radius));
        return true;
    }

    Ray generateRandomRay() override {
        Vector3f ret;
        do {
            ret = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
        }   while(ret.squaredLength() >= 1.0);
        ret.normalize();
        Vector3f ret2;
        do {
            ret2 = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
        }   while(ret2.squaredLength() >= 1.0);

        return Ray(center+radius*ret, (ret2 + ret).normalized());
    }

protected:
    Vector3f center;
    float radius;
    Vector3f getNorm(float u, float v, const Vector3f &hit_point, const Vector3f &norm) {
        if (!material->getBump()->hasBump()) {
            return norm;
        }
        float value = 0;
        Vector2f grad = material->getBump()->GradAt(u, v, value);
        float phi = u * 2 * M_PI, theta = M_PI - v * M_PI;
        Vector3f du(-hit_point.z(), 0, hit_point.x()), dv(hit_point.y() * cos(phi), -radius * sin(theta), hit_point.y() * sin(phi));
        if (du.squaredLength() < FLT_EPSILON) return norm;
        return Vector3f::cross(du + norm.normalized() * grad[0] / (2 * M_PI), dv + norm.normalized() * grad[1] / M_PI);
    }

};


#endif
