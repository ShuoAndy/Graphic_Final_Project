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
		//构造函数
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(a - b, a - c).normalized();
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		if (fabs( Vector3f::dot(normal, ray.getDirection())) < 1e-6) return false;
		Vector3f E1 = vertices[0] - vertices[1];
		Vector3f E2 = vertices[0] - vertices[2];
		Vector3f S = vertices[0] - ray.getOrigin();//忠实地还原课件内容

		float det1 = Matrix3f(S, E1, E2).determinant();//调用计算三元矩阵行列式的函数
		float det2 = Matrix3f(ray.getDirection(), S, E2).determinant();
		float det3 = Matrix3f(ray.getDirection(), E1, S).determinant();
		float det = Matrix3f(ray.getDirection(), E1, E2).determinant();

		float t = det1 / det;
		float beta = det2 / det;
		float gama = det3 / det;
		
		Vector3f hit_point = ray.pointAtParameter(t);
		float v = Vector3f::dot(hit_point, vdim);
        float u = Vector3f::dot(hit_point, udim);

		if (t > 0 && t < hit.getT() && t > tmin && beta >= 0 && beta <= 1 && gama >= 0 && gama <= 1 && beta + gama <= 1)
		{
			if (Vector3f::dot(normal, ray.getDirection()) < 0)
				hit.set(t, u, v, this->material, normal, ray);
			else
				hit.set(t, u, v, this->material, -normal, ray);
			return true;
		}
		else
			return false;
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
