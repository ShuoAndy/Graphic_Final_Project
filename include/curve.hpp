#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>
#include <iostream>
using namespace std;
// TODO (PA3): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve {
protected:
    std::vector<Vector3f> controls;
public:
    float ymin, ymax, xzmax, mu_min, mu_max;
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {
        ymin = 0x3f3f3f3f;
        ymax = -0x3f3f3f3f;
        xzmax = 0;
        for (auto pt : controls) {
            ymin = min(pt.y(), ymin);
            ymax = max(pt.y(), ymax);
            xzmax = max(xzmax, fabs(pt.x()));
            xzmax = max(xzmax, fabs(pt.z()));
        }
    }


    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
    virtual CurvePoint getPoint(float mu) = 0;

    void evaluate(float mu, int k, std::vector<float> knots, std::vector<float> upknots, std::vector<float>& pts, std::vector<float>& dpts){
        int bpos = 0;
        pts.clear();
        dpts.clear();
        if (upknots[0] != mu)
            bpos = max(0l, lower_bound(upknots.begin(), upknots.end(), mu) - upknots.begin() - 1);
        else
            bpos = upper_bound(upknots.begin(), upknots.end(), mu) - upknots.begin() - 1;
        
        std::vector<float> s(k + 2);
        std::vector<float> ds(k + 1, 1);
        std::vector<float> ret(controls.size(),0);
        std::vector<float> dret(controls.size(),0);
        s[k] = 1;
        for (int p = 1; p <= k; p ++)
            for (int ii = k - p; ii <= k; ii ++){
                int i = ii + bpos - k;
                int j = i + p;
                if (i < 0){
                    i = (i + knots.size()) % knots.size();
                }
                float w1, dw1, w2, dw2;
                if (knots[j] == knots[i]){
                    w1 = mu;
                    dw1 = 1;
                } else {
                    w1 = (mu - knots[i]) / (knots[j] - knots[i]);
                    dw1 = 1.0f / (knots[j] - knots[i]);
                }
                if (knots[j + 1] == knots[i + 1]){
                    w2 = 1.0 - mu;
                    dw2 = -1;
                } else {
                    w2 = (knots[j + 1] - mu) / (knots[j + 1] - knots[i + 1]);
                    dw2 = - 1.0f / (knots[j + 1] - knots[i + 1]);
                }
                if (p == k)
                    ds[ii] = (dw1 * s[ii] + dw2 * s[ii + 1]) * p;
                s[ii] = w1 * s[ii] + w2 * s[ii + 1];
            }
        int lsk = bpos - k;
        int rsk = controls.size() - bpos - 1;
        int start = lsk;
        if (lsk < 0){
            lsk = -lsk;
            start = 0;
        } else {
            start = lsk;
            lsk = 0;
        }
        if (rsk >= 0)
            rsk = 0;

        for (int i = lsk, j = start; i < k + 1 + rsk; i ++, j ++)
            ret[j] = s[i];

        for (int i = lsk, j = start; i < ds.size() + rsk; i ++, j ++)
            dret[j] = ds[i];

        pts = ret; 
        dpts = dret;
    }
    
    CurvePoint getSinglePoint(float mu, int k, std::vector<float> knots, std::vector<float> upknots) {
        CurvePoint pt;
        int bpos = upper_bound(knots.begin(), knots.end(), mu) - knots.begin() - 1;
        //if (upknots[0] != mu) bpos = max(0l, lower_bound(upknots.begin(), upknots.end(), mu) - upknots.begin() - 1);
        std::vector<float> s(k + 2);
        std::vector<float> ds(k + 1, 1);
        s[k] = 1;
        for (int p = 1; p <= k; ++p) {
            for (int ii = k - p; ii < k + 1; ++ii) {
                int i = ii + bpos - k;
                float w1, dw1, w2, dw2;
                if (upknots[i + p] == upknots[i]) {
                    w1 = mu;
                    dw1 = 1;
                } else {
                    w1 = (mu - upknots[i]) / (upknots[i + p] - upknots[i]);
                    dw1 = 1.0 / (upknots[i + p] - upknots[i]);
                }
                if (upknots[i + p + 1] == upknots[i + 1]) {
                    w2 = 1 - mu;
                    dw2 = -1;
                } else {
                    w2 = (upknots[i + p + 1] - mu) /
                         (upknots[i + p + 1] - upknots[i + 1]);
                    dw2 = -1 / (upknots[i + p + 1] - upknots[i + 1]);
                }
                if (p == k) ds[ii] = (dw1 * s[ii] + dw2 * s[ii + 1]) * p;
                s[ii] = w1 * s[ii] + w2 * s[ii + 1];
            }
        }
        s.pop_back();
        int lsk = k - bpos, rsk = bpos + 1 - controls.size();
        if (lsk > 0) {
            for (int i = lsk; i < s.size(); ++i) {
                s[i - lsk] = s[i];
                ds[i - lsk] = ds[i];
            }
            s.resize(s.size() - lsk);
            ds.resize(ds.size() - lsk);
            lsk = 0;
        }
        if (rsk > 0) {
            if (rsk < s.size()) {
                s.resize(s.size() - rsk);
                ds.resize(ds.size() - rsk);
            } else {
                s.clear();
                ds.clear();
            }
        }
        for (int j = 0; j < s.size(); ++j) {
            pt.V += controls[-lsk + j] * s[j];
            pt.T += controls[-lsk + j] * ds[j];
        }
        return pt;
    }

};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        mu_min = 0;
        mu_max = 1;
    }

    CurvePoint getPoint(float mu) {
        int k = controls.size() - 1;
        std::vector<float> knots(controls.size());
        for (int i = 0; i < controls.size(); i ++)
            knots.push_back(1); 
        std::vector<float> upknots = knots;
        float last = knots[knots.size() - 1];
        for (int i = 0; i < k; i ++)
            knots.push_back(last);
        return getSinglePoint(mu, k, knots, upknots);
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        int k = controls.size() - 1;
        std::vector<float> knots(controls.size());
        for (int i = 0; i < controls.size(); i ++)
            knots.push_back(1); 
        std::vector<float> upknots = knots;
        std::vector<float> samples;
        int start = k;
        int end = k + 1;
        for (int i = start; i < upknots.size() - end; i ++){
            if (upknots[i + 1] > upknots[i]){
                for (int j = 0; j < resolution; j ++)
                    samples.push_back(upknots[i] + (upknots[i + 1] - upknots[i]) * ((float) j) / ((float) resolution));
            }
        }
        float last = knots[knots.size() - 1];
        for (int i = 0; i < k; i ++)
            knots.push_back(last);
        std::vector<float> pts;
        std::vector<float> dpts;
        std::vector<CurvePoint> ret; 
        for (float sample: samples){
            Vector3f point(0,0,0);
            Vector3f dpoint(0,0,0);
            evaluate(sample, k, knots, upknots, pts, dpts);
            for (int j = 0; j < controls.size(); j ++)
                point = point + pts[j] * controls[j];
            for (int j = 0; j < controls.size(); j ++)
                dpoint = dpoint + dpts[j] * controls[j];
            dpoint.normalize();
            ret.push_back((CurvePoint){point, dpoint});
        }
        data = ret;
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        mu_min = 3.f / (float(points.size()) + 3.f);
        mu_max = float(points.size()) / (float(points.size()) + 3.f);
    }

    CurvePoint getPoint(float mu) {
        int k = 3;
        std::vector<float> knots;
        for (int i = 0; i <= controls.size() + k; i ++)
            knots.push_back(((float) i)/((float) controls.size() + k)); 
        std::vector<float> upknots = knots;
        float last = knots[knots.size() - 1];
        for (int i = 0; i < k; i ++)
            knots.push_back(last);
        return getSinglePoint(mu, k, knots, upknots);
    } 

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        int k = 3;
        std::vector<float> knots;
        for (int i = 0; i <= controls.size() + k; i ++)
            knots.push_back(((float) i)/((float) controls.size() + k)); 
        std::vector<float> upknots = knots;
        std::vector<float> samples;
        int start = k;
        int end = k + 1;
        for (int i = start; i < upknots.size() - end; i ++){
            if (upknots[i + 1] > upknots[i]){
                for (int j = 0; j < resolution; j ++)
                    samples.push_back(upknots[i] + (upknots[i + 1] - upknots[i]) * ((float) j) / ((float) resolution));
            }
        }
        float last = knots[knots.size() - 1];
        for (int i = 0; i < k; i ++)
            knots.push_back(last);
        std::vector<float> pts;
        std::vector<float> dpts;
        std::vector<CurvePoint> ret; 
        for (float sample: samples){
            Vector3f point(0,0,0);
            Vector3f dpoint(0,0,0);
            evaluate(sample, k, knots, upknots, pts, dpts);
            for (int j = 0; j < controls.size(); j ++)
                point = point + pts[j] * controls[j];
            for (int j = 0; j < controls.size(); j ++)
                dpoint = dpoint + dpts[j] * controls[j];
            dpoint.normalize();
            ret.push_back((CurvePoint){point, dpoint});
        }
        data = ret;
    }

protected:

};

#endif // CURVE_HPP
