#ifndef PTSPPM_H
#define PTSPPM_H

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <omp.h>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <vector>
#include <cstring>
#include <string>

using namespace std;

class Integrator {
public:
    Integrator(SceneParser* sceneParser){
        this->sceneParser = sceneParser;
    }
    virtual void render(char* outputFile) = 0;

protected:
    SceneParser* sceneParser;

};

//实现Path tracing 算法

class PTIntegrator: public Integrator{
public:
    PTIntegrator(SceneParser* sceneParser, int num_sample):Integrator(sceneParser) {
        this->num_sample = num_sample;
    }

    virtual void render(char* outputFile) {
        Camera* camera = sceneParser->getCamera();
        int height = camera->getHeight(), width = camera->getWidth();
        Image image(width, height);
        printf("started rendering \n");
        omp_set_num_threads(25);
#pragma omp parallel for schedule(dynamic, 1)  // openMP并行加速
        for (int x = 0; x < width; x ++){
            fprintf(stderr, "rendering row %d with thread %d\n", x, omp_get_thread_num());
            for (int y = 0; y < height; y ++){
                Vector3f finalColor = Vector3f::ZERO;
                unsigned short Xi[3]={0,0,y*y*y};

                // 实现抗锯齿效果        
                for(int sx=0;sx<2;sx++){
                    for(int sy=0;sy<2;sy++){

                        for (int s = 0; s < num_sample; s ++)
                        {
                            double r1=2*erand48(Xi), dx=(r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1)); 
                            double r2=2*erand48(Xi), dy=(r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2));
                            float u = float(x+ (sx-.5 + dx)/2);
                            float v = float(y+ (sy-.5 + dy)/2);
                            Ray camRay = camera->generateRay(Vector2f(u, v));
                            finalColor += radiance(camRay, 0) / float(4);
                        }
                    }
                }
                
                
                finalColor = finalColor / float(num_sample);
                image.SetPixel(x, y, finalColor);
            }
        }
        printf("rendering finished\n");
        image.SaveImage(outputFile);
    }

private:
    int num_sample;

    Vector3f radiance(Ray& camRay, int depth,  Vector3f attenuation_new = Vector3f(1,1,1))
    {
        //基于smallpt实现的pathtracing

        Group* baseGroup = sceneParser->getGroup();
        Hit hit;
        bool isIntersect = false;
        if (baseGroup != nullptr)
            isIntersect = baseGroup->intersect(camRay, hit, 0.0001);
        
        if(!isIntersect)
            return sceneParser->getBackgroundColor(camRay); //直接返回背景色

        Vector3f finalColor = hit.getMaterial()->getSpecularColor();

        Ray scattered(camRay);

        Vector3f attenuation;

        bool scatter_success = hit.getMaterial()->Scatter(camRay, hit, attenuation, scattered);
        
        attenuation_new = attenuation_new * attenuation;


        if (depth < 50 && scatter_success)
        {
            return finalColor + attenuation*radiance(scattered,  depth + 1, attenuation_new); //递归计算散射光
        }

        return finalColor;
    }

};

#endif