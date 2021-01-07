#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "photonkdtree.hpp"

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

class PTIntegrator: public Integrator{
public:
    PTIntegrator(SceneParser* sceneParser, int num_sample, int use_v2):Integrator(sceneParser) {
        this->num_sample = num_sample;
        this->use_v2 = use_v2;
        if (use_v2) printf("switch to v2 renderer \n");
    }

    virtual void render(char* outputFile) {
        Camera* camera = sceneParser->getCamera();
        int height = camera->getHeight(), width = camera->getWidth();
        Image image(width, height);
        time_t start_time = time(NULL);
        printf("started rendering \n");
        omp_set_num_threads(25);
#pragma omp parallel for schedule(dynamic, 1) 
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
        printf("rendering finished in %d secs\n", end_time - start_time);
        image.SaveImage(outputFile);
    }

private:
    int num_sample;
    bool use_v2;
    Vector3f get_color(float x, float y, Camera* camera, int depth_limit=20){
        Ray camRay = camera->generateRay(Vector2f(x, y));
        if (use_v2) {
            return pathTracingV2(camRay, camera, 0, depth_limit);
        }
        else
            return pathTracing(camRay, camera, 0, depth_limit);
    }
    Vector3f pathTracing(Ray& camRay, Camera* camera, int depth, int depth_limit=50, Vector3f total_atten = Vector3f(1,1,1)){
        Group* baseGroup = sceneParser->getGroup();
        Hit hit;
        bool isIntersect = false;
        if (baseGroup != nullptr)
            isIntersect = baseGroup->intersect(camRay, hit, 0.0001);
        if (isIntersect){
            Vector3f finalColor = hit.getMaterial()->Emission();
            Ray scattered(camRay);
            Vector3f attenuation;
            bool scatter_success = hit.getMaterial()->Scatter(camRay, hit, attenuation, scattered);
            total_atten = total_atten * attenuation;
            if (total_atten.x() < 1e-3 && total_atten.y() < 1e-3 && total_atten.z() < 1e-3) {
                return finalColor;
            }
            if (depth < depth_limit && scatter_success){
                return finalColor + attenuation*pathTracing(scattered, camera, depth + 1, depth_limit, total_atten);
            }
            return finalColor;
        } else {
            return sceneParser->getBackgroundColor(camRay);
        }
    }

    // move all scatter logic into pt method 
    Vector3f pathTracingV2(Ray& camRay, Camera* camera, int depth, int depth_limit=50, Vector3f total_atten = Vector3f(1,1,1)){
        Group* baseGroup = sceneParser->getGroup();
        Hit hit;
        bool isIntersect = false;
        if (baseGroup != nullptr)
            isIntersect = baseGroup->intersect(camRay, hit, 0.0001);

        if (isIntersect){
            Material* mat = hit.getMaterial();
            Vector3f normal = hit.normal;
            Vector3f finalColor = mat->Emission();
            Ray scattered(camRay);
            Vector3f attenuation;
            bool scatter_success = true; 
            if (!strcmp(mat->name(), "diff")) {

                Vector3f target = normal + mat->generateRandomPoint();
                scattered = Ray(hit.point, target);
                attenuation = mat->getAttenuation(hit.u, hit.v);

            } else if (!strcmp(mat->name(), "met")) {

                Vector3f input = camRay.getDirection().normalized();
                Vector3f target = input - 2*Vector3f::dot(input, normal) * normal;
                scattered = Ray(camRay.pointAtParameter(hit.t), target + mat->fuzz * mat->generateRandomPoint());
                attenuation = mat->getAttenuation(hit.u, hit.v);

            } else if (!strcmp(mat->name(), "die")) {

                attenuation = mat->getAttenuation(hit.u, hit.v);
                float refractive = mat->refractive;
                Vector3f unit = camRay.getDirection().normalized();
                Vector3f target = unit - 2*Vector3f::dot(unit, normal) * normal;
                float index, reflect_prob, cos = -1*Vector3f::dot(unit, normal);
                Vector3f refract_ray;

                if (!hit.is_front_face){
                    index = refractive;
                } else {
                    index = 1.0 / refractive;
                }

                bool can_refract = false;
                float t = -1*Vector3f::dot(unit, normal);
                float disc = 1.0 - index * index * (1 - t * t);

                if (disc > 0){
                    refract_ray = index * (unit + t * normal) - normal * sqrt(disc);
                    can_refract = true;
                }

                if (can_refract){
                    float r = ((1 - index) / (1 + index)) * ((1 - index) / (1 + index));
                    reflect_prob = r + (1 - r) * pow((1 - cos), 5);
                } else {
                    reflect_prob = 1.1;
                }

                if (drand48() < reflect_prob){
                    scattered = Ray(camRay.pointAtParameter(hit.t), target);
                } else {
                    scattered = Ray(camRay.pointAtParameter(hit.t), refract_ray);
                }

            } else if (!strcmp(mat->name(), "diffuse_light")) {
                scatter_success = false;
            } else if (!strcmp(mat->name(), "medium")) {
                scattered = Ray(hit.point, mat->generateRandomPoint());
                attenuation = mat->attenuation;
            }

            if (depth < depth_limit && scatter_success){
                total_atten = total_atten * attenuation;
                if (total_atten.x() < 1e-3 && total_atten.y() < 1e-3 && total_atten.z() < 1e-3) {
                    return finalColor;
                }
                return finalColor + attenuation*pathTracing(scattered, camera, depth + 1, depth_limit, total_atten);
            }
            return finalColor;

        } else {
            return sceneParser->getBackgroundColor(camRay);
        }
    }


};

class SPPMIntegrator: public Integrator {
    public:
        SPPMIntegrator(SceneParser* sceneParser, char* ckpt_dir, int steps_to_save, int num_round, int num_photon, int use_v2): Integrator(sceneParser), ckpt_dir(ckpt_dir), steps_to_save(steps_to_save), num_round(num_round), num_photon(num_photon){
            KDTreeRoot = nullptr;
            this->use_v2 = use_v2;
            if (use_v2) fprintf(stderr, "switch to v2 renderer \n");
        }

        virtual void render(char* outputFile) {
            Camera* camera = sceneParser->getCamera();
            int height = camera->getHeight(), width = camera->getWidth();
            Group* baseGroup = sceneParser->getGroup();
            light_sources = baseGroup->getLightSources();
            Image image(width, height);
            time_t start_time = time(NULL);
            for (int i = 0; i < width; i ++)
                for (int j = 0; j < height; j ++)
                    visible_points.push_back(new Hit()); 
            printf("started rendering \n");
            omp_set_num_threads(25);
            for (int i = 0; i < num_round; i ++){
                fprintf(stderr, "sppm round %d / %d \n", i, num_round);
#pragma omp parallel for schedule(dynamic, 1)
                for (int x = 0; x < width; x ++){
                    if (x % 100 == 0)
                        fprintf(stderr, "tracing row %d for visible points with thread %d round %d\n", x, omp_get_thread_num(), i);
                    for (int y = 0; y < height; y ++){
                        float u = float(x + drand48());
                        float v = float(y + drand48());
                        Ray camRay = camera->generateRay(Vector2f(u,v));
                        visible_points[x * height + y]->setT(1e38);
                        if (use_v2) getVisiblePointsV2(camRay, visible_points[x * height + y]);
                        else getVisiblePoints(camRay, visible_points[x * height + y]);
                    }
                }
                buildTree();
                int avg_photons = num_photon / light_sources.size();
#pragma omp parallel for schedule(dynamic, 1)
                for (int x = 0; x < avg_photons; x ++){
                    if (x % 100000 == 0)
                        fprintf(stderr, "tracing photon %d with thread %d round %d\n", x, omp_get_thread_num(), i);
                    for (int y = 0; y < light_sources.size(); y ++){
                        Ray r = light_sources[y]->generateRandomRay();
                        if (use_v2) photonTracingV2(r, light_sources[y]->getMaterial()->Emission());
                        else photonTracing(r, light_sources[y]->getMaterial()->Emission());
                    }
                }

                if ((i + 1) % steps_to_save == 0) {
                    fprintf(stderr, "save checkpoints ...\n");
                    char file[1024];
                    sprintf(file, "ckpt-%d.bmp", i + 1);
                    saveImage(file, i + 1, num_photon);
                }                

            }
            saveImage("final.bmp", num_round, num_photon);
            
        }

    
    private:
        char* ckpt_dir;
        int steps_to_save;
        int num_round;
        int num_photon;
        int use_v2;
        vector<Object3D*> light_sources;
        vector<Hit*> visible_points;
        PhotonKDTreeNode* KDTreeRoot;

        // Collect all visible points alone the camera ray
        void getVisiblePoints(Ray &ray, Hit* hit, int depth_limit = 20) {
            Group* baseGroup = sceneParser->getGroup();
            if (baseGroup == nullptr)
                return;
            int depth = 0;
            Vector3f total_atten(1,1,1);
            while(depth < depth_limit) {
                depth ++;
                hit->setT(1e38);

                // Doesn't hit anything, stop tracing
                if (!baseGroup->intersect(ray, *hit, 0.0001)){
                    hit->LightFlux += hit->Attenuation * sceneParser->getBackgroundColor(ray);
                    return;
                }

                Ray scattered(ray);
                Vector3f attenuation;
                bool scatter_success = hit->getMaterial()->Scatter(ray, *hit, attenuation, scattered);
                if (scatter_success){
                    total_atten = total_atten * attenuation;
                }

                // hit a diffuse material (including a light material)
                if (strcmp(hit->getMaterial()->name(), "diff") == 0 || strcmp(hit->getMaterial()->name(), "diffuse_light") == 0){
                    hit->Attenuation = total_atten;
                    hit->LightFlux += total_atten * hit->getMaterial()->Emission();
                    return;
                }
                ray = scattered;
            }
        }

        void getVisiblePointsV2(Ray &ray, Hit* hit, int depth_limit = 20) {
            Group* baseGroup = sceneParser->getGroup();
            if (baseGroup == nullptr)
                return;
            int depth = 0;
            Vector3f total_atten(1,1,1);
            while(depth < depth_limit && (total_atten.x() >= 1e-3 || total_atten.y() >= 1e-3 || total_atten.z() >= 1e-3)) {
                depth ++;
                hit->setT(1e38);

                // Doesn't hit anything, stop tracing
                if (!baseGroup->intersect(ray, *hit, 0.0001)){
                    hit->LightFlux += hit->Attenuation * sceneParser->getBackgroundColor(ray);
                    return;
                }

                Material* mat = hit->material;
                Vector3f normal = hit->normal;
                Vector3f finalColor = mat->Emission();
                Ray scattered(ray);
                Vector3f attenuation;
                bool scatter_success = true; 
                if (!strcmp(mat->name(), "diff")) {

                    hit->Attenuation = total_atten * mat->attenuation;
                    hit->LightFlux += total_atten * finalColor;
                    return;

                } else if (!strcmp(mat->name(), "met")) {

                    Vector3f input = ray.getDirection().normalized();
                    Vector3f target = input - 2*Vector3f::dot(input, normal) * normal;
                    scattered = Ray(ray.pointAtParameter(hit->t), target + mat->fuzz * mat->generateRandomPoint());
                    attenuation = mat->getAttenuation(hit->u, hit->v);

                } else if (!strcmp(mat->name(), "die")) {

                    attenuation = mat->getAttenuation(hit->u, hit->v);
                    float refractive = mat->refractive;
                    Vector3f unit = ray.getDirection().normalized();
                    Vector3f target = unit - 2*Vector3f::dot(unit, normal) * normal;
                    float index, reflect_prob, cos = -1*Vector3f::dot(unit, normal);
                    Vector3f refract_ray;

                    if (!hit->is_front_face){
                        index = refractive;
                    } else {
                        index = 1.0 / refractive;
                    }

                    bool can_refract = false;
                    float t = -1*Vector3f::dot(unit, normal);
                    float disc = 1.0 - index * index * (1 - t * t);

                    if (disc > 0){
                        refract_ray = index * (unit + t * normal) - normal * sqrt(disc);
                        can_refract = true;
                    }

                    if (can_refract){
                        float r = ((1 - index) / (1 + index)) * ((1 - index) / (1 + index));
                        reflect_prob = r + (1 - r) * pow((1 - cos), 5);
                    } else {
                        reflect_prob = 1.1;
                    }

                    if (drand48() < reflect_prob){
                        scattered = Ray(ray.pointAtParameter(hit->t), target);
                    } else {
                        scattered = Ray(ray.pointAtParameter(hit->t), refract_ray);
                    }

                } else if (!strcmp(mat->name(), "diffuse_light")) {

                    hit->Attenuation = total_atten;
                    hit->LightFlux += total_atten * finalColor;
                    return;

                } else if (!strcmp(mat->name(), "medium")) {

                    scattered = Ray(hit->point, mat->generateRandomPoint());
                    attenuation = mat->attenuation;

                }
                ray = scattered;
                total_atten = total_atten * attenuation;
            }
        }

        void photonTracing(Ray& ray, const Vector3f& radiance, int depth_limit = 20){
            Group* baseGroup = sceneParser->getGroup();
            if (baseGroup == nullptr)
                return;
            int depth = 0;
            Vector3f total_atten(radiance);
            total_atten = total_atten * Vector3f(255, 255, 255);
            while(depth < depth_limit) {
                depth ++;
                Hit hit;
                // Doesn't hit anything, stop tracing
                if (!baseGroup->intersect(ray, hit, 0.0001)){
                    return;
                }
                Ray scattered(ray);
                Vector3f attenuation;
                bool scatter_success = hit.getMaterial()->Scatter(ray, hit, attenuation, scattered);

                // hit a diffuse material
                if (strcmp(hit.getMaterial()->name(), "diff") == 0){
                    KDTreeRoot->Update(hit.getPoint(), total_atten, hit.isFrontFace());
                }
                if (scatter_success){
                    total_atten = total_atten * attenuation;
                }
                if (!scatter_success) return;

                ray = scattered;
            }
        }

        void photonTracingV2(Ray& ray, const Vector3f& radiance, int depth_limit = 20){
            Group* baseGroup = sceneParser->getGroup();
            if (baseGroup == nullptr)
                return;
            int depth = 0;
            Vector3f total_atten(radiance);
            total_atten = total_atten * Vector3f(255, 255, 255);
            while(depth < depth_limit  && (total_atten.x() >= 1e-3 || total_atten.y() >= 1e-3 || total_atten.z() >= 1e-3)) {
                depth ++;
                Hit hit;
                // Doesn't hit anything, stop tracing
                if (!baseGroup->intersect(ray, hit, 0.0001)){
                    return;
                }

                Material* mat = hit.getMaterial();
                Vector3f normal = hit.normal;
                Vector3f finalColor = mat->Emission();
                Ray scattered(ray);
                Vector3f attenuation;
                bool scatter_success = true; 
                if (!strcmp(mat->name(), "diff")) {

                    KDTreeRoot->Update(hit.getPoint(), total_atten, hit.isFrontFace());
                    Vector3f target = normal + mat->generateRandomPoint();
                    scattered = Ray(hit.point, target);
                    attenuation = mat->getAttenuation(hit.u, hit.v);

                } else if (!strcmp(mat->name(), "met")) {

                    Vector3f input = ray.getDirection().normalized();
                    Vector3f target = input - 2*Vector3f::dot(input, normal) * normal;
                    scattered = Ray(ray.pointAtParameter(hit.t), target + mat->fuzz * mat->generateRandomPoint());
                    attenuation = mat->getAttenuation(hit.u, hit.v);

                } else if (!strcmp(mat->name(), "die")) {

                    attenuation = mat->getAttenuation(hit.u, hit.v);
                    float refractive = mat->refractive;
                    Vector3f unit = ray.getDirection().normalized();
                    Vector3f target = unit - 2*Vector3f::dot(unit, normal) * normal;
                    float index, reflect_prob, cos = -1*Vector3f::dot(unit, normal);
                    Vector3f refract_ray;

                    if (!hit.is_front_face){
                        index = refractive;
                    } else {
                        index = 1.0 / refractive;
                    }

                    bool can_refract = false;
                    float t = -1*Vector3f::dot(unit, normal);
                    float disc = 1.0 - index * index * (1 - t * t);

                    if (disc > 0){
                        refract_ray = index * (unit + t * normal) - normal * sqrt(disc);
                        can_refract = true;
                    }

                    if (can_refract){
                        float r = ((1 - index) / (1 + index)) * ((1 - index) / (1 + index));
                        reflect_prob = r + (1 - r) * pow((1 - cos), 5);
                    } else {
                        reflect_prob = 1.1;
                    }

                    if (drand48() < reflect_prob){
                        scattered = Ray(ray.pointAtParameter(hit.t), target);
                    } else {
                        scattered = Ray(ray.pointAtParameter(hit.t), refract_ray);
                    }

                } else if (!strcmp(mat->name(), "diffuse_light")) {
                    scatter_success = false;
                } else if (!strcmp(mat->name(), "medium")) {
                    scattered = Ray(hit.point, mat->generateRandomPoint());
                    attenuation = mat->attenuation;
                }

                if (scatter_success){
                    total_atten = total_atten * attenuation;
                }
                if (!scatter_success) return;

                ray = scattered;
            }
        }

        void buildTree(){
            printf("building KDTree for SPPM \n");
            if (KDTreeRoot != nullptr)
                clearTree(KDTreeRoot);
            vector<Hit*> hit_list(visible_points);
            printf("total visible points %d \n", hit_list.size());
            KDTreeRoot = new PhotonKDTreeNode(hit_list, 0, hit_list.size() - 1, 0);
            printf("building finished \n");
        }

        void clearTree(PhotonKDTreeNode* node) {
            if (node != nullptr){
                if (node->left_node) clearTree(node->left_node);
                if (node->right_node) clearTree(node->right_node);
                delete node;
            }
        }

        void saveImage(const char* filename, int rounds, int photons) {
            Camera* camera = sceneParser->getCamera();
            int height = camera->getHeight(), width = camera->getWidth();
            Image image(width, height);
            for (int u = 0; u < width; ++u)
                for (int v = 0; v < height; ++v) {
                    Hit* hit = visible_points[u * height + v];
                    image.SetPixel(u, v, hit->PhotonFlux / (M_PI * hit->Radius * num_photon * rounds) + hit->LightFlux / rounds);
                }
            char abs_path[1024];
            strcpy(abs_path, ckpt_dir);
            strcat(abs_path, "/");
            strcat(abs_path, filename);
            fprintf(stderr, "save image to path %s \n", abs_path);
            image.SaveImage(abs_path);
        }
};

#endif