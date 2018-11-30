#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>

#include "hw2_types.h"
#include "hw2_file_ops.h"
#include "tinymath.h"



Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

tinymath::vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;



/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;
        }
}



void forwardRenderingPipeline(Camera cam) {
    // TODO: IMPLEMENT HERE
}

int main(int argc, char **argv) {
    
    return 0;
}
