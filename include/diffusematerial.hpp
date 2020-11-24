#ifndef DIFFUSEMATERIAL_H
#define DIFFUSEMATERIAL_H
#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include <iostream>

class DiffuseMaterial: public Material{
    public:
        DiffuseMaterial(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0):Material(d_color, s_color, atten, s){}
        Vector3f Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
            Vector3f target = ray.pointAtParameter(hit.getT()) + hit.getNormal() + generateRandomPoint();
            scattered = Ray(ray.pointAtParameter(hit.getT()), target - ray.pointAtParameter(hit.getT()));
            attenuation = this->attenuation;
            return true;
        }
        char* name(){
            return "diff";
        }
};

class MetalMaterial: public Material{
    public:
        MetalMaterial(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, float fuzz = 0):Material(d_color, s_color, atten, s){ this->fuzz = fuzz > 1 ? 1 : fuzz;}
        Vector3f Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
            Vector3f target = reflectRay(ray.getDirection().normalized(), hit.getNormal());
            scattered = Ray(ray.pointAtParameter(hit.getT()), target + fuzz * generateRandomPoint());
            attenuation = this->attenuation;
            return (Vector3f::dot(scattered.getDirection(), hit.getNormal()) > 0);
        }
        char* name(){
            return "met";
        }
    private:
        float fuzz;
        Vector3f reflectRay(const Vector3f& input, const Vector3f& normal){
            return input - 2*Vector3f::dot(input, normal) * normal;
        }
};

#endif