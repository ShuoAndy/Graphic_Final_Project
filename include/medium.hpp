#ifndef MEDIUM_H
#define MEDIUM_H

#include "object3d.hpp"
#include "material.hpp"
#include "ray.hpp"

class Isotropic : public Material {
    public:
        Isotropic(Vector3f c) {
            this->attenuation = c;
        }
        virtual bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered) override {
            scattered = Ray(hit.getPoint(), generateRandomPoint());
            attenuation = this->attenuation;
            return true;
        }
        char* name() {
            return "medium";
        }
};

class Medium: public Object3D {

public:
    Medium(Object3D* obj, float density, Vector3f color): obj(obj), NegInvDense(-1/density)
    {
        this->material = new Isotropic(color);
        is_infinite = obj->is_infinite;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) override {
        Hit h1, h2;

        if (!obj->intersect(r, h1, -0x3f3f3f3f)) return false;
        if (!obj->intersect(r, h2, h1.t + 0.0001)) return false;

        if (h1.t < tmin) h1.t = tmin;

        if (h1.t >= h2.t)
            return false;

        if (h1.t < 0)
            h1.t = 0;

        float ray_length = r.getDirection().length();
        float distance_inside_boundary = (h2.t - h1.t) * ray_length;
        float hit_distance = NegInvDense * log(drand48());

        if (hit_distance > distance_inside_boundary)
            return false;

        h.set(h1.t + hit_distance / ray_length, 0, 0, material, Vector3f::ZERO, r);

        return true;
        
    }

    virtual bool getBox(Box& box) override {
        return obj->getBox(box);
    }

    Object3D* obj;
    float NegInvDense;
};


#endif