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
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
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
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
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

class DielecMaterial: public Material{
    public:
        DielecMaterial(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, float refractive = 0):Material(d_color, s_color, atten, s){ this->refractive = refractive;}
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
            attenuation = Vector3f(1.0,1.0,1.0);
            Vector3f target = reflectRay(ray.getDirection().normalized(), hit.getNormal());
            float index, reflect_prob, cos;
            Vector3f out_normal, refract_ray;
            if (Vector3f::dot(ray.getDirection(), hit.getNormal()) > 0){
                out_normal = -hit.getNormal();
                index = refractive;
                cos = refractive * Vector3f::dot(ray.getDirection().normalized(), hit.getNormal().normalized());
            } else {
                out_normal = hit.getNormal();
                index = 1.0 / refractive;
                cos = -1* Vector3f::dot(ray.getDirection().normalized(), hit.getNormal().normalized());
            }

            if (refractRay(ray.getDirection(), out_normal, index, refract_ray)){
                reflect_prob = Schlick(cos, refractive);
            } else {
                reflect_prob = 1.1;
            }
            if (drand48() < reflect_prob){
                scattered = Ray(ray.pointAtParameter(hit.getT()), target);
            } else {
                scattered = Ray(ray.pointAtParameter(hit.getT()), refract_ray);
            }
            return true;
        }

        char* name(){
            return "die";
        }

    private:
        float refractive;
        Vector3f reflectRay(const Vector3f& input, const Vector3f& normal){
            return input - 2*Vector3f::dot(input, normal) * normal;
        }
        bool refractRay(const Vector3f& input, const Vector3f& normal, float index, Vector3f& refracted){
            Vector3f unit = input.normalized();
            float t = Vector3f::dot(unit, normal);
            float disc = 1.0 - index * index * (1 - t * t);
            if (disc > 0){
                refracted = index * (unit - t * normal) - normal * sqrt(disc);
                return true;
            }
            return false;
        }
        float Schlick(float cos, float index){
            float r = ((1 - index) / (1 + index)) * ((1 - index) / (1 + index));
            return r + (1 - r) * pow((1 - cos), 5);
        }
};

#endif