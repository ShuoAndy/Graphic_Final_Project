#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		this->vertices[0] = a;
		this->vertices[1] = b;
		this->vertices[2] = c;
		this->normal = Vector3f::cross(b - a, c - a);
		this->normal.normalize();
		this->d = Vector3f::dot(this->normal, a);
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f r_dir = ray.getDirection();

        if(abs(Vector3f::dot(r_dir.normalized(), this->normal.normalized())) < 1e-10)
            return false;

        float t = (this->d - Vector3f::dot(this->normal, ray.getOrigin()))/(Vector3f::dot(this->normal, ray.getDirection()));
        
		if (t < tmin || t > hit.getT())
            return false;

		Vector3f inter_point = ray.pointAtParameter(t);
		for (int i = 0; i < 3; i ++){
			Vector3f line = inter_point - this->vertices[i];
			Vector3f line_a = this->vertices[(i+1)%3] - this->vertices[i];
			Vector3f line_b = this->vertices[(i+2)%3] - this->vertices[i];
			if (Vector3f::dot(Vector3f::cross(line, line_a), Vector3f::cross(line, line_b)) > 0)
				return false;
		}

		if (Vector3f::dot(ray.getDirection(), this->normal) > 0)
            hit.set(t, this->material, -this->normal.normalized());
        else
            hit.set(t, this->material, this->normal.normalized());

        return true;
	}
	Vector3f normal;
	Vector3f vertices[3];
protected:
	float d;
};

#endif //TRIANGLE_H
