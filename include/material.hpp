#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.h"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &atten = Vector3f::ZERO, float s = 0, const char* texture_name = "", const char* bump_name = "") :
            diffuseColor(d_color), specularColor(s_color), attenuation(atten), shininess(s) {
            texture = Texture(texture_name);
            bump = BumpTexture(bump_name);
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        // 已完成
        Vector3f N = hit.getNormal().normalized();
        Vector3f L = dirToLight.normalized();
        Vector3f V = -(ray.getDirection().normalized());
        Vector3f R = (2 * Vector3f::dot(N, L) * N - L).normalized();
        Vector3f diffuse;
        Vector3f specular;
        if(Vector3f::dot(L, N)>0)
            diffuse = diffuseColor * (Vector3f::dot(L, N));
        else
            diffuse = Vector3f::ZERO;
        
        if (Vector3f::dot(V, R) > 0)
            specular = specularColor * pow(Vector3f::dot(V, R), shininess);
        else
            specular = Vector3f::ZERO;
        
        shaded = lightColor * (diffuse + specular);
        return shaded;
    }

    
    virtual char* name(){
        return "non";
    }
    inline virtual bool Scatter(const Ray &ray, const Hit &hit, Vector3f& attenuation, Ray& scattered)=0;

    virtual Vector3f Emission() const {
        return specularColor;
    }
    
    virtual float getRefractive() {
        return 0.0;
    }

    virtual Vector3f getAttenuation(float u, float v) {
        return this->attenuation;
    }

    BumpTexture* getBump() {
        return &bump;
    }
    
    Vector3f diffuseColor;  //漫反射系数
    Vector3f specularColor; //镜面反射系数
    Vector3f attenuation;   //衰减系数
    float fuzz;     //模糊度
    float shininess;    //光泽度
    float refractive;   //折射率
    Texture texture;    //纹理
    BumpTexture bump;

    
    Vector3f random_in_unit_sphere(){
        Vector3f ret;
        do {
            ret = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
        }   while(ret.squaredLength() >= 1.0);
        return ret;
    }

};


#endif // MATERIAL_H
