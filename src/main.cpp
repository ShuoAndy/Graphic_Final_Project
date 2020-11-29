#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

Vector3f pathTracing(Ray& camRay, SceneParser& sceneParser, Camera* camera, int depth, int depth_limit=50){
    Group* baseGroup = sceneParser.getGroup();
    Hit hit;
    bool isIntersect = false;
    if (baseGroup != nullptr)
        isIntersect = baseGroup->intersect(camRay, hit, 0.001);
    if (isIntersect){
        Vector3f finalColor = Vector3f::ZERO;
        Ray scattered(camRay);
        Vector3f attenuation;
        hit.getMaterial()->Scatter(camRay, hit, attenuation, scattered);
        if (depth < 50 && hit.getMaterial()->Scatter(camRay, hit, attenuation, scattered)){
            return attenuation*pathTracing(scattered, sceneParser, camera, depth + 1, depth_limit);
        }
        return finalColor;
    } else {
        Vector3f bc = sceneParser.getBackgroundColor(camRay);
        return sceneParser.getBackgroundColor(camRay);
    }
}
Vector3f get_color(float x, float y, SceneParser& sceneParser, Camera* camera, int depth_limit=50){
    Ray camRay = camera->generateRay(Vector2f(x, y));
    return pathTracing(camRay, sceneParser, camera, 0, depth_limit);
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
    int sn = 10; //for anti-aliasing
    // Then loop over each pixel in the image, shooting a ray
    //progress bar
    char progress_bar[102];
    memset(progress_bar, '\0', sizeof(char) * 102);//
    int len = 0;
    int i = 0;
    char ch[4] = { '|' ,'/' ,'-','\\'};

    for (int x = 0; x < width; x ++)
        for (int y = 0; y < height; y ++){
            Vector3f finalColor = Vector3f::ZERO;
            len = (101 * (x * height + y)) / (width * height) + 1;
            printf("[%-100s]%d%%%c\r", progress_bar, len, ch[len%4]);
            progress_bar[len - 1] = '=';
            for (int s = 0; s < sn; s ++){
                float u = float(x + drand48());
                float v = float(y + drand48());
                finalColor += get_color(u, v, sceneParser, camera);
            }
            finalColor = finalColor / float(sn);
            image.SetPixel(x, y, finalColor);
            fflush(stdout);
        }
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    image.SaveImage(outputFile);
    return 0;
}

