#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <omp.h>
#include <cstdio>
#include <iostream>
#include <ctime>

class Integrator {
public:
    Integrator(SceneParser* sceneParser){
        this->sceneParser = sceneParser;
    }
    virtual void render(char* outputFile) = 0;

protected:
    SceneParser* sceneParser;

};

class PTIntegrator: public Integrator{
public:
    PTIntegrator(SceneParser* sceneParser, int num_sample):Integrator(sceneParser) {
        this->num_sample = num_sample;
    }

    virtual void render(char* outputFile) {
        Camera* camera = sceneParser->getCamera();
        int height = camera->getHeight(), width = camera->getWidth();
        Image image(width, height);
        time_t start_time = time(NULL);
        printf("started rendering \n");
        omp_set_num_threads(25);
#pragma omp parallel for schedule(dynamic, 1)  // OpenMP
        for (int x = 0; x < width; x ++){
            fprintf(stderr, "rendering row %d with thread %d\n", x, omp_get_thread_num());
            for (int y = 0; y < height; y ++){
                Vector3f finalColor = Vector3f::ZERO;
                for (int s = 0; s < num_sample; s ++){
                    float u = float(x + drand48());
                    float v = float(y + drand48());
                    finalColor += get_color(u, v, camera);
                }
                finalColor = finalColor / float(num_sample);
                image.SetPixel(x, y, finalColor);
            }
        }
        time_t end_time = time(NULL);
        printf("rendering finished in %d secs", end_time - start_time);
        image.SaveImage(outputFile);
    }

private:
    int num_sample;
    Vector3f get_color(float x, float y, Camera* camera, int depth_limit=50){
        Ray camRay = camera->generateRay(Vector2f(x, y));
        return pathTracing(camRay, camera, 0, depth_limit);
    }
    Vector3f pathTracing(Ray& camRay, Camera* camera, int depth, int depth_limit=50, Vector3f total_atten = Vector3f(1,1,1)){
        Group* baseGroup = sceneParser->getGroup();
        Hit hit;
        bool isIntersect = false;
        if (baseGroup != nullptr)
            isIntersect = baseGroup->intersect(camRay, hit, 0.001);
        if (isIntersect){
            Vector3f finalColor = hit.getMaterial()->Emission();
            Ray scattered(camRay);
            Vector3f attenuation;
            hit.getMaterial()->Scatter(camRay, hit, attenuation, scattered);
            total_atten = total_atten * attenuation;
            if (total_atten.x() < 1e-3 && total_atten.y() < 1e-3 && total_atten.z() < 1e-3) {
                return finalColor;
            }
            if (depth < depth_limit && hit.getMaterial()->Scatter(camRay, hit, attenuation, scattered)){
                return finalColor + attenuation*pathTracing(scattered, camera, depth + 1, depth_limit);
            }
            return finalColor;
        } else {
            return sceneParser->getBackgroundColor(camRay);
        }
    }
};

#endif