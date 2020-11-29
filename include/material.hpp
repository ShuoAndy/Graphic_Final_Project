#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), attenuation(atten), shininess(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        //
        Vector3f diffuse = this->diffuseColor;
        float factor = Vector3f::dot(dirToLight, hit.getNormal());
        diffuse = diffuse * std::max(factor, (float)0);
        factor = Vector3f::dot(-ray.getDirection(), 2 * Vector3f::dot(dirToLight, hit.getNormal()) * hit.getNormal() - dirToLight);
        if (factor > 0){
            Vector3f specular = this->specularColor * powf(factor , this->shininess);
            shaded = lightColor * (diffuse + specular);
        } else {
            shaded = lightColor * diffuse;
        }
        return shaded;
    }
    virtual char* name(){
        return "mat";
    }
    virtual bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered)=0;

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    Vector3f attenuation;
    float shininess;
    Vector3f generateRandomPoint(){
        Vector3f ret;
        do {
            ret = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
        }   while(ret.squaredLength() >= 1.0);
        return ret;
    }
};


#endif // MATERIAL_H
