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
    // Then loop over each pixel in the image, shooting a ray
    for (int x = 0; x < width; x ++)
        for (int y = 0; y < height; y ++){
            Ray camRay = camera->generateRay(Vector2f(x, y));
            cout << camRay << endl;
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
                image.SetPixel(x, y, finalColor);
            } else {
                Vector3f bc = sceneParser.getBackgroundColor(camRay);

                image.SetPixel(x, y, sceneParser.getBackgroundColor(camRay));
            }
        }
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    image.SaveImage(outputFile);
    return 0;
}

