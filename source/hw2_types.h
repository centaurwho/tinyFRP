#include "tinymath.h"

#ifndef HW2_TYPES_H
#define HW2_TYPES_H

typedef struct {
    int cameraId;
    tinymath::vec3 pos;
    tinymath::vec3 gaze;
    tinymath::vec3 v;
    tinymath::vec3 u;
    tinymath::vec3 w;
    double l, r, b, t;
    double n;
    double f;
    int sizeX;
    int sizeY;
    char outputFileName[80];
} Camera;

typedef struct {
    double r, g, b;
} Color;

typedef struct {
    int vertexIds[3];
} Triangle;

typedef struct {
    int modelId;
    int type;
    int numberOfTransformations;
    int transformationIDs[1000];
    char transformationTypes[1000];
    int numberOfTriangles;
    Triangle triangles[25000];

    std::vector<int> usedVertices;
} Model;

#endif //HW2_TYPES_H
