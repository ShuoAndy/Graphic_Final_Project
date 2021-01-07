#ifndef RECTANGLES_H
#define RECTANGLES_H

#include "object3d.hpp"
#include "box.hpp"
#include <vecmath.h>
#include <cmath>


class XYRectangle: public Object3D {
    public:
        XYRectangle() {}

        XYRectangle(float x0, float y0, float x1, float y1, float d, float n, Material* mat): Object3D(mat), x0(x0), y0(y0), x1(x1), y1(y1), d(d), n(n) {}

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {

            if (fabs(r.getDirection().z()) < 1e-10) return false;

            auto t = (d-r.getOrigin().z()) / r.getDirection().z();

            if (t < tmin || t > h.getT())
                return false;

            auto x = r.getOrigin().x() + t*r.getDirection().x();
            auto y = r.getOrigin().y() + t*r.getDirection().y();
            if (x < x0 || x > x1 || y < y0 || y > y1)
                return false;
            
            h.set(t, (x-x0)/(x1-x0), (y-y0)/(y1-y0), getMaterial(), Vector3f(0, 0, 1), r);

            return true;

        }

        virtual bool getBox(Box &box) override {
            box = Box(Vector3f(x0,y0, d - 0.0001), Vector3f(x1, y1, d + 0.0001));
            return true;
        }

        Ray generateRandomRay() override {
            float u = drand48(), v = drand48();
            u = x0 + (x1 - x0) * u;
            v = y0 + (y1 - y0) * v;

            Vector3f ret2;
            do {
                ret2 = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
            }   while(ret2.squaredLength() >= 1.0);
            return Ray(Vector3f(u, v, d), (ret2 + n*Vector3f(0, 0, 1)).normalized());
        }
    
    private:
        float x0, y0, x1, y1, d, n;
};

class XZRectangle: public Object3D {
    public:
        XZRectangle() {}

        XZRectangle(float x0, float z0, float x1, float z1, float d, float n, Material* mat): Object3D(mat), x0(x0), z0(z0), x1(x1), z1(z1), d(d), n(n) {}

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {

            if (fabs(r.getDirection().y()) < 1e-10) return false;

            auto t = (d-r.getOrigin().y()) / r.getDirection().y() ;

            if (t < tmin || t > h.getT())
                return false;

            auto x = r.getOrigin().x() + t*r.getDirection().x();
            auto z = r.getOrigin().z() + t*r.getDirection().z();
            if (x < x0 || x > x1 || z < z0 || z > z1)
                return false;
            
            h.set(t, (x-x0)/(x1-x0), (z-z0)/(z1-z0), getMaterial(), n*Vector3f(0, 1, 0), r);

            return true;
        }
        
        virtual bool getBox(Box &box) override {
            box = Box(Vector3f(x0, d - 0.0001, z0), Vector3f(x1, d + 0.0001, z1));
            return true;
        }

        Ray generateRandomRay() override {
            float u = drand48(), v = drand48();
            u = x0 + (x1 - x0) * u;
            v = z0 + (z1 - z0) * v;

            Vector3f ret2;
            do {
                ret2 = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
            }   while(ret2.squaredLength() >= 1.0);
            return Ray(Vector3f(u, d, v), (ret2 + n*Vector3f(0, 1, 0)).normalized());
        }
    
    private:
        float x0, z0, x1, z1, d, n;
};

class YZRectangle: public Object3D {
    public:
        YZRectangle() {}

        YZRectangle(float y0, float z0, float y1, float z1, float d, float n, Material* mat): Object3D(mat), y0(y0), z0(z0), y1(y1), z1(z1), d(d), n(n) {}

        virtual bool intersect(const Ray &r, Hit &h, float tmin) override {

            if (fabs(r.getDirection().x()) < 1e-10) return false;

            auto t = (d-r.getOrigin().x()) / r.getDirection().x() ;

            if (t < tmin || t > h.getT())
                return false;

            auto y = r.getOrigin().y() + t*r.getDirection().y();
            auto z = r.getOrigin().z() + t*r.getDirection().z();
            if (y < y0 || y > y1 || z < z0 || z > z1)
                return false;
            
            h.set(t, (y-y0)/(y1-y0), (z-z0)/(z1-z0), getMaterial(), n * Vector3f(1, 0, 0), r);

            return true;
        }
        
        virtual bool getBox(Box &box) override {
            box = Box(Vector3f(d - 0.0001, y0, z0), Vector3f(d + 0.0001, y1, z1));
            return true;
        }

        Ray generateRandomRay() override {
            float u = drand48(), v = drand48();
            
            u = z0 + (z1 - z0) * u;
            v = y0 + (y1 - y0) * v;

            Vector3f ret2;
            do {
                ret2 = 2.0 * Vector3f(drand48(), drand48(), drand48()) - Vector3f(1,1,1);
            }   while(ret2.squaredLength() >= 1.0);

            return Ray(Vector3f(u, v, d), (ret2 + n*Vector3f(1, 0, 0)).normalized());
        }
    
    private:
        float y0, z0, y1, z1, d, n;
};

#endif