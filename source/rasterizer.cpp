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


void modellingTransform(Camera cam, int modelId) { 
    //TODO
}


void cameraTransform(Camera cam) {
    tinymath::vec3 u = cam.u;
    tinymath::vec3 v = cam.v;
    tinymath::vec3 w = cam.w;

    
    tinymath::matrix camRotation = tinymath::getMRotation(u,v,w);
    tinymath::matrix camTranslate = tinymath::getTranslationMatrix((-1)*cam.pos);

    tinymath::matrix camMatrix = tinymath::matrixMultMatrix(camRotation,camTranslate);

    tinymath::vec4 camPos4d = tinymath::vec4(cam.pos);
    
    //TODO: Handle colorid. Do we add it into vec4?
    int colorId = cam.pos.colorId;
    
    tinymath::vec4 newPos = tinymath::matrixMultVec4(camPos4d,camMatrix); //newPos vec3 or vec4
    cam.pos = tinymath::vec3(newPos);
    cam.pos.colorId = colorId;

}

void perspectiveTransform(Camera cam, int modelId) { //map to CVV
    tinymath::matrix perspectiveMatrix;

    double n = cam.n;
    double f = cam.f;
    double l = cam.l;
    double r = cam.r;
    double b = cam.b;
    double t = cam.t;
    double rminusl = r-l;
    double tminusb = t-b;
    double fminusn = f-n;

    perspectiveMatrix[0][0] = (2*n)/rminusl;
    perspectiveMatrix[0][2] = (r+l)/rminusl;
    perspectiveMatrix[1][1] = (2*n)/tminusb;
    perspectiveMatrix[1][2] = (t+b)/tminusb;
    perspectiveMatrix[2][2] = -(f+n)/fminusn;
    perspectiveMatrix[2][3] = (-2*f*n)/fminusn;
    perspectiveMatrix[3][2] = -1.0;
    perspectiveMatrix[3][3] = 0.0;

    
    //return 
}

void viewportTransform(Camera cam, int modelId) { //map to 2d
    //TODO
    tinymath::matrix vpMatrix();

}

void forwardRenderingPipeline(Camera cam) {
    modellingTransform(cam,0);

}

int main(int argc, char **argv) {
    //tinymath::vec4 vec = tinymath::vec4(1,2,3,4);
    //tinymath::matrix mat = tinymath::getTranslationMatrix(Translation(1,2,3));

    //tinymath::printVec4(tinymath::matrixMultVec4(vec, mat));
    return 0;
}
