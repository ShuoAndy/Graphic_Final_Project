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
		has_smooth_norm = has_tex = false;
		if (Vector3f::cross(this->normal.normalized(), Vector3f::UP).length() < 1e-6)
            this->vdim = Vector3f::FORWARD;
        else this->vdim = Vector3f::UP;
        this->udim = Vector3f::cross(this->normal.normalized(), this->vdim);
		Vector3f min_point(fmin(vertices[0].x(), fmin(vertices[1].x(), vertices[2].x())), fmin(vertices[0].y(), fmin(vertices[1].y(), vertices[2].y())), fmin(vertices[0].z(), fmin(vertices[1].z(), vertices[2].z())));
		Vector3f max_point(fmax(vertices[0].x(), fmax(vertices[1].x(), vertices[2].x())), fmax(vertices[0].y(), fmax(vertices[1].y(), vertices[2].y())), fmax(vertices[0].z(), fmax(vertices[1].z(), vertices[2].z())));
		box = Box(min_point - 0.0001f, max_point + 0.0001f);
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f r_dir = ray.getDirection();

        if(abs(Vector3f::dot(r_dir.normalized(), this->normal.normalized())) < 1e-10)
            return false;

        float t = (this->d - Vector3f::dot(this->normal, ray.getOrigin()))/(Vector3f::dot(this->normal, ray.getDirection()));
        
		if (t < tmin || t > hit.getT() || isinf(t) || isnan(t) )
            return false;

		Vector3f inter_point = ray.pointAtParameter(t);
		for (int i = 0; i < 3; i ++){
			Vector3f line = inter_point - this->vertices[i];
			Vector3f line_a = this->vertices[(i+1)%3] - this->vertices[i];
			Vector3f line_b = this->vertices[(i+2)%3] - this->vertices[i];
			if (Vector3f::dot(Vector3f::cross(line, line_a), Vector3f::cross(line, line_b)) > 0)
				return false;
		}

		Vector3f hit_point = ray.pointAtParameter(t);
		float v = Vector3f::dot(hit_point, vdim);
        float u = Vector3f::dot(hit_point, udim);
		Vector3f norm = this->normal;

		if (has_tex) {
			Vector3f va = (vertices[0] - hit_point), vb = (vertices[1] - hit_point), vc = (vertices[2] - hit_point);
			float ra = Vector3f::cross(vb, vc).length(), rb = Vector3f::cross(vc, va).length(), rc = Vector3f::cross(va, vb).length();
			Vector2f uv = (ra * at + rb * bt + rc * ct) / (ra + rb + rc);
			u = uv.x();
			v = uv.y();
		}

        hit.set(t, u, v, this->material, norm, ray);

        return true;
	}
	
	bool getBox(Box& box){
		box = this->box;
		return true;
	}

	void setSmoothedNorm(const Vector3f& anorm, const Vector3f& bnorm,
                  const Vector3f& cnorm) {
        an = anorm;
        bn = bnorm;
        cn = cnorm;
        has_smooth_norm = true;
    }

    void setTexCoord(const Vector2f& _at, const Vector2f& _bt, const Vector2f& _ct) {
        at = _at;
        bt = _bt;
        ct = _ct;
        has_tex = true;
    }

	Vector3f normal;
	Vector3f vertices[3];
	Vector3f vdim, udim, an, bn, cn;
	Vector2f at, bt, ct;
	Box box;
	bool has_tex, has_smooth_norm;

protected:
	float d;
};

#endif //TRIANGLE_H
