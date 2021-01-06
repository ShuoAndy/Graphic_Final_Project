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
#include "integrator.hpp"

#include <string>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 6) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file/dir> <accelerator name> <number of sample/round> <integrator type>" << endl;
        return 1;
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
    } else if (strcmp(integrator, "sppm") == 0) {
        printf("constructing SPPM Integraotor... \n");
        SPPMIntegrator integrator(&sceneParser, outputFile, 10, num_sample, 1000000);
        printf("integrator successfullt constructed. \n");
        integrator.render(outputFile);
    }

    return 0;
}

