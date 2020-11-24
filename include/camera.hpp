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
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        this->angle = angle;    
    }

    Ray generateRay(const Vector2f &point) override {
        // 
        float height = this->getHeight(), width = this->getWidth();
        float length = ((float)this->getHeight() / 2.f) / sin(angle / 2.f);
        float factor = sqrt(length*length - height*height * 0.25f - width*width * 0.25f);
        Vector3f camera_ray(point.x() - width / 2.f, height / 2.f - point.y(), factor);
        camera_ray.normalize();
        Matrix3f trans(this->horizontal.normalized(), -this->up.normalized(), this->direction.normalized(), true);
        Vector3f dir = trans * camera_ray;
        dir.normalize();
        return Ray(this->center, dir);
    }

protected:
    float angle;
};

#endif //CAMERA_H
