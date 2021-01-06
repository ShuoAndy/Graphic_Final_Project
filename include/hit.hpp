#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        t = 1e38;
        PhotonFlux = LightFlux = Vector3f::ZERO;
        Attenuation = Vector3f(1.0);
        Radius = 0.01;
        PhotonCount = 0;
    }

    Hit(float _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
        PhotonFlux = LightFlux = Vector3f::ZERO;
        Attenuation = Vector3f(1.0);
        Radius = 0.0001;
        PhotonCount = 0;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        PhotonFlux = LightFlux = h.LightFlux;
        Attenuation = h.Attenuation;
        Radius = h.Radius;
        PhotonCount = 0;
    }

    // destructor
    ~Hit() = default;

    float getT() const {
        return t;
    }

    float getU() const {
        return u;
    }

    float getV() const {
        return v;
    }
    
    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }
    
    Vector3f getPoint() const {
        return point;
    }

    bool isFrontFace() const {
        return is_front_face;
    }

    void setT(float _t) {
        t = _t;
    }
    
    void set(float _t, float _u, float _v, Material *m, const Vector3f &n, const Ray &r) {
        t = _t;
        u = _u;
        v = _v;
        material = m;
        is_front_face = Vector3f::dot(r.getDirection(), n) <= 0.0;
        normal = is_front_face ? n : -n;
        point = r.pointAtParameter(t);
    }
    Vector3f LightFlux, PhotonFlux, Attenuation;
    float Radius;
    int PhotonCount;

private:
    float t;
    float u, v;
    Material *material;
    Vector3f normal;
    Vector3f point;
    bool is_front_face;

};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
