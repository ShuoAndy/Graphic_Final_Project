#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <float.h>
// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f(0, 0, 0);
        radius = 1;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        // 构造函数
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
       // 已完成
        Vector3f l = center - r.getOrigin();//计算向量
        float tp = Vector3f::dot(l, r.getDirection().normalized());//注意尽量使用normalized，其会返回标准化向量
        if( l.length()>radius && tp<0 )//对应ppt第37页
            return false;

        float d = sqrt(l.squaredLength() - tp * tp);
        if( d>radius)//对应ppt第38页
            return false;

        float t0 = sqrt(radius * radius - d * d);
        float t;
        if(l.length()>radius)
            t = tp - t0;
        else if(l.length()<radius)
            t = t0 + tp;
        //至此，t已计算完毕

        Vector3f out_norm;
        out_norm =  r.pointAtParameter(t) - this->center;   //计算向外的法向量并标准化
        out_norm.normalize();
        float u = atan2(-out_norm.z(), out_norm.x()) / (2 * M_PI) + 0.5f; //这里的计算详见https://blog.csdn.net/aa20274270/article/details/52709444
        float v = acos(-out_norm.y()) / M_PI;   //u和v分别是方位角和极角

        if (t < h.getT() && t > tmin)
        {
            if (l.length()>radius)
                h.set(t, u, v, material, (r.pointAtParameter(t) - center).normalized(),r); //注意HIT结构体的normal属性是标准法向
            else
                h.set(t, u, v,material, (center - r.pointAtParameter(t)).normalized(),r);
            return true;
        }
        else
            return false;
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

};

//实现运动模糊，详见ray tracing the next week
//定义一个“运动球”

class moving_sphere : public Sphere
{
public:
    moving_sphere() {}
    moving_sphere(Vector3f c0, Vector3f c1, double _time0, double _time1, double r, Material *mat)
    {
        radius = r;
        material = mat;
        center0 = c0;
        center1 = c1;
        time0 = _time0;
        time1 = _time1;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
       // 已完成
        Vector3f l = GetCenter(r.time()) - r.getOrigin();//计算向量
        float tp = Vector3f::dot(l, r.getDirection().normalized());//注意尽量使用normalized，其会返回标准化向量
        if( l.length()>radius && tp<0 )//对应ppt第37页
            return false;

        float d = sqrt(l.squaredLength() - tp * tp);
        if( d>radius)//对应ppt第38页
            return false;

        float t0 = sqrt(radius * radius - d * d);
        float t;
        if(l.length()>radius)
            t = tp - t0;
        else if(l.length()<radius)
            t = t0 + tp;
        //至此，t已计算完毕

        Vector3f out_norm;
        out_norm =  r.pointAtParameter(t) - this->GetCenter(r.time());   //计算向外的法向量并标准化
        out_norm.normalize();
        float u = atan2(-out_norm.z(), out_norm.x()) / (2 * M_PI) + 0.5f; //这里的计算详见https://blog.csdn.net/aa20274270/article/details/52709444
        float v = acos(-out_norm.y()) / M_PI;   //u和v分别是方位角和极角

        if (t < h.getT() && t > tmin)
        {
            if (l.length()>radius)
                h.set(t, u, v, material, (r.pointAtParameter(t) - GetCenter(r.time())).normalized(),r); //注意HIT结构体的normal属性是标准法向
            else
                h.set(t, u, v,material, (GetCenter(r.time()) - r.pointAtParameter(t)).normalized(),r);
            return true;
        }
        else
            return false;
    }
    
    bool getBox(Box& box) override {
        Box box1=Box(GetCenter(time0) - Vector3f(radius, radius, radius), GetCenter(time0)+Vector3f(radius, radius, radius));
        Box box2=Box(GetCenter(time1) - Vector3f(radius, radius, radius), GetCenter(time1)+Vector3f(radius, radius, radius));
        box = mergeBox(box1,box2);
        return true;
    }

    Vector3f GetCenter(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

    Vector3f center0, center1;
    double time0, time1;
};

#endif
