#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include <iostream>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle, float lens_radius, float focus_distance) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        this->angle = angle;    
        this->lens_radius = lens_radius;
        float h = tan(angle / 2.0);
        float w = h * (float(imgW) / float(imgH));
        this->focus_plain_corner = this->center - h * focus_distance * this->up.normalized() - w * focus_distance * this->horizontal.normalized() + focus_distance * this->direction.normalized();
        this->plain_width = 2 * w * focus_distance * this->horizontal;
        this->plain_height = 2 * h * focus_distance * this->up;
    }
    
    Vector3f random_in_unit_sphere() //Ray Tracing in one weekend中生成单位球内向量的方法
    {
        Vector3f p;
        do {
            p = 2.0 * Vector3f(drand48(), drand48(), 0) - Vector3f(1,1,0);
        } while (Vector3f::dot(p, p) >= 1.0);
        return p;
    }

    Ray generateRay(const Vector2f &point) override {
        // 已完成
        Vector3f random = lens_radius * random_in_unit_sphere();
        Vector3f offset = horizontal * random.x() + up * random.y();
        return Ray(this->center + offset, this->focus_plain_corner + float(point.x())/float(this->width) * this->plain_width + float(point.y())/float(this->height) * this->plain_height - this->center - offset);
    }

protected:
    float angle;

    //实现景深 DOF
    float lens_radius;
    Vector3f focus_plain_corner;
    Vector3f plain_width;
    Vector3f plain_height;
    
};

#endif //CAMERA_H
