#ifndef DIFFUSEMATERIAL_H
#define DIFFUSEMATERIAL_H
#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include <iostream>

class DiffuseLight: public Material{
    public:
        DiffuseLight(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, const char* texture_name = "", const char* bump_name = ""):Material(d_color, s_color, atten, s, texture_name, bump_name){}
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scatteredv){
            return false;
        }
        virtual Vector3f Emission() const override {
            return specularColor;
        }
        char* name(){
            return "diffuse_light";
        }
};

class DiffuseMaterial: public Material{
    public:
        DiffuseMaterial(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, const char* texture_name = "", const char* bump_name = ""):Material(d_color, s_color, atten, s, texture_name, bump_name){}
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
            Vector3f target = ray.pointAtParameter(hit.getT()) + hit.getNormal() + generateRandomPoint();
            scattered = Ray(ray.pointAtParameter(hit.getT()), target - ray.pointAtParameter(hit.getT()));
            attenuation = getAttenuation(hit.getU(), hit.getV());
            return true;
        }
        char* name(){
            return "diff";
        }
    private:
        Vector3f getAttenuation(float u, float v) {
            if (texture.hasTexture()) return texture.ColorAt(u ,v);
            return this->attenuation;
        }
};

class MetalMaterial: public Material{
    public:
        MetalMaterial(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, const char* texture_name = "", const char* bump_name = "", float fuzz = 0):Material(d_color, s_color, atten, s, texture_name, bump_name){ this->fuzz = fuzz > 1 ? 1 : fuzz;}
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
            Vector3f target = reflectRay(ray.getDirection().normalized(), hit.getNormal());
            scattered = Ray(ray.pointAtParameter(hit.getT()), target + fuzz * generateRandomPoint());
            attenuation = getAttenuation(hit.getU(), hit.getV());
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
        Vector3f getAttenuation(float u, float v) {
            if (texture.hasTexture()) return texture.ColorAt(u ,v);
            return this->attenuation;
        }
};

class DielecMaterial: public Material{
    public:
        DielecMaterial(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, const char* texture_name = "", const char* bump_name = "", float refractive = 0):Material(d_color, s_color, atten, s, texture_name, bump_name){ this->refractive = refractive;}
        bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered){
            attenuation = this->attenuation;
            Vector3f unit = ray.getDirection().normalized();
            Vector3f target = reflectRay(unit, hit.getNormal());
            float index, reflect_prob, cos = -1*Vector3f::dot(unit, hit.getNormal());
            Vector3f refract_ray;
            if (!hit.isFrontFace()){
                index = refractive;
            } else {
                index = 1.0 / refractive;
            }

            if (refractRay(unit, hit.getNormal(), index, refract_ray)){
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
            Vector3f unit = input;
            float t = -1*Vector3f::dot(unit, normal);
            float disc = 1.0 - index * index * (1 - t * t);
            if (disc > 0){
                refracted = index * (unit + t * normal) - normal * sqrt(disc);
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