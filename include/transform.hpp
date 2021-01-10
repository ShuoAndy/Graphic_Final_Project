#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

// TODO: implement this class so that the intersect function first transforms the ray
class Transform : public Object3D {
public:
    Transform() {}

    Transform(const Matrix4f &m, Object3D *obj) : o(obj) {
        transform = m.inverse();
        is_infinite = true;
    }

    ~Transform() {
    }

    virtual Material* getMaterial() override {
        return o->getMaterial();
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
        Vector3f trSource = transformPoint(transform, r.getOrigin());
        Vector3f trDirection = transformDirection(transform, r.getDirection());
        Ray tr(trSource, trDirection);
        bool inter = o->intersect(tr, h, tmin);
        if (inter) {
            h.set(h.getT(), h.getU(), h.getV(), h.getMaterial(), transformDirection(transform.transposed(), h.getNormal()).normalized(), r);
        }
        return inter;
    }
    virtual bool getBox(Box& box) override {
        Box objBox;
        if(!o->getBox(objBox))
            return false;
        Vector3f min_point = transformPoint(transform, objBox.min());
        Vector3f max_point = transformPoint(transform, objBox.max());
        box = Box(min_point, max_point);
        return true;
    }

protected:
    Object3D *o; //un-transformed object
    Matrix4f transform;
};

#endif //TRANSFORM_H
