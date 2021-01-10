#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "box.hpp"
#include "curve.hpp"
#include "object3d.hpp"
#include "triangle.hpp"

const int steps = 20;
const float eps = 1e-4;

class RevSurface : public Object3D {
   Curve *pCurve;
   Box box;
   public:
    RevSurface(Curve *pCurve, Material *material)
        : pCurve(pCurve), Object3D(material) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        box = Box(Vector3f(-pCurve->xzmax, pCurve->ymin - 3, -pCurve->xzmax), Vector3f(pCurve->xzmax, pCurve->ymax + 3, pCurve->xzmax));
    }


    ~RevSurface() override { delete pCurve; }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        float t, theta, mu;
        if (!getInitIntersect(r, t) || t > h.t) return false;
        Vector3f pt(r.origin + r.direction * t);
        theta = atan2(-pt.z(), pt.x()) + M_PI;
        mu = (pCurve->ymax - pt.y()) / (pCurve->ymax - pCurve->ymin);
        Vector3f normal, point;
        if (!NG(r, t, theta, mu, normal)) {
            return false;
        }
        if (isnan(mu) || isnan(theta) || isnan(t)) return false;
        if (isinf(mu) || isinf(theta) || isinf(t)) return false;
        if (t < tmin || mu < pCurve->mu_min || mu > pCurve->mu_max || t > h.getT()) return false;

        h.set(t, theta / (2 * M_PI), mu, material, normal.normalized(), r);
        return true;
    }

    bool NG(const Ray &r, float &t, float &theta, float &mu,
                Vector3f &normal) {
        Vector3f dmu, dtheta;
        for (int i = 0; i < steps; ++i) {

            if (theta < 0.0) theta += 2 * M_PI;
            if (theta >= 2 * M_PI) theta = fmod(theta, 2 * M_PI);
            if (mu >= pCurve->mu_max) mu = pCurve->mu_max - FLT_EPSILON;
            if (mu <= pCurve->mu_min) mu = pCurve->mu_min + FLT_EPSILON;
            Vector3f point;
            Quat4f rot;
            rot.setAxisAngle(theta, Vector3f::UP);
            Matrix3f rotMat = Matrix3f::rotation(rot);
            CurvePoint cp = pCurve->getPoint(mu);
            point = rotMat * cp.V;
            dmu = rotMat * cp.T;
            dtheta = Vector3f(-cp.V.x() * sin(theta), 0, -cp.V.x() * cos(theta));
            Vector3f f = r.origin + r.direction * t - point;
            float dist2 = f.squaredLength();
            normal = Vector3f::cross(dmu, dtheta);
            if (dist2 < eps) return true;
            float D = Vector3f::dot(r.direction, normal);
            t -= Vector3f::dot(dmu, Vector3f::cross(dtheta, f)) / D;
            mu -= Vector3f::dot(r.direction, Vector3f::cross(dtheta, f)) / D;
            theta += Vector3f::dot(r.direction, Vector3f::cross(dmu, f)) / D;

        }
        return false;
    }


    virtual bool getBox(Box &box) override {
        box = this->box;
        return true;
    }

    bool getInitIntersect(const Ray &r, float & t) {
        float inv = 1.0f / r.getDirection().x();
        Vector3f min_point = box.min();
        Vector3f max_point = box.max();
        float t_min = -0x3f3f3f3f;
        float t_max = 0x3f3f3f3f;
        float t0 = (min_point.x() - r.getOrigin().x()) * inv;
        float t1 = (max_point.x() - r.getOrigin().x()) * inv;
        if (inv < 0.0f) {
            std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min) 
            return false;

        inv = 1.0f / r.getDirection().y();
        t0 = (min_point.y() - r.getOrigin().y()) * inv;
        t1 = (max_point.y() - r.getOrigin().y()) * inv;
        if (inv < 0.0f) {
            std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min) 
            return false;
        
        inv = 1.0f / r.getDirection().z();
        t0 = (min_point.z() - r.getOrigin().z()) * inv;
        t1 = (max_point.z() - r.getOrigin().z()) * inv;
        if (inv < 0.0f) {
            std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min) 
            return false;
        t = t_min;
        return true;
    }
};

#endif  // REVSURFACE_HPP
