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
#include "pt.hpp"

#include <string>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    char* integrator = argv[5];
    char* inputFile = argv[1];
    char* outputFile = argv[2];  // only bmp is allowed.
    char* accelerator = argv[3];
    int num_sample = atoi(argv[4]);

    cout << "parse scene ..." << endl;
    SceneParser sceneParser(inputFile, accelerator);
    cout << "parse scene successfully" << endl;
    if (strcmp(integrator, "pt") == 0) {
        printf("constructing Path tracing integrator... \n");
        PTIntegrator integrator(&sceneParser, num_sample);
        printf("integrator successfullt constructed. \n");
        integrator.render(outputFile);
    } 

    return 0;
}

