#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

Vector3f get_color(float x, float y, SceneParser& sceneParser, Camera* camera){
    Ray camRay = camera->generateRay(Vector2f(x, y));
    Group* baseGroup = sceneParser.getGroup();
    Hit hit;

    bool isIntersect = false;
    if (baseGroup != nullptr)
        isIntersect = baseGroup->intersect(camRay, hit, 0);
    if (isIntersect){
        Vector3f finalColor = Vector3f::ZERO;
        int numberLights = sceneParser.getNumLights();
        for (int li = 0; li < numberLights; li ++){
            Light* light = sceneParser.getLight(li);
            Vector3f L, lightColor;
            light->getIllumination(camRay.pointAtParameter(hit.getT()), L, lightColor);
            finalColor += hit.getMaterial()->Shade(camRay, hit, L, lightColor);
        }
        return finalColor;
    } else {
        Vector3f bc = sceneParser.getBackgroundColor(camRay);
        return sceneParser.getBackgroundColor(camRay);
    }
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    char* inputFile = argv[1];
    char* outputFile = argv[2];  // only bmp is allowed.

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    SceneParser sceneParser(inputFile);
    Camera* camera = sceneParser.getCamera();
    int height = camera->getHeight(), width = camera->getWidth();
    Image image(width, height);
    int sn = 100; //for anti-aliasing
    // Then loop over each pixel in the image, shooting a ray
    for (int x = 0; x < width; x ++)
        for (int y = 0; y < height; y ++){
            Vector3f finalColor(0, 0, 0);
            for (int s = 0; s < sn; s ++){
                float u = float(x + drand48());
                float v = float(y + drand48());
                finalColor += get_color(u, v, sceneParser, camera);
            }
            finalColor = finalColor / float(sn);
            image.SetPixel(x, y, finalColor);
        }
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    image.SaveImage(outputFile);
    return 0;
}

