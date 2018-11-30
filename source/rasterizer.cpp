#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <algorithm>


#include "hw2_types.h"
#include "hw2_file_ops.h"
#include "tinymath.h"


Camera cameras[100];
int numberOfCameras;

Model models[1000];
int numberOfModels;

Color colors[100000];
int numberOfColors;

Translation translations[1000];
int numberOfTranslations;

Rotation rotations[1000];
int numberOfRotations;

Scaling scalings[1000];
int numberOfScalings;

tinymath::vec3 vertices[100000];
int numberOfVertices;

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


void modellingTransform() { 
    std::vector<tinymath::vec3> transModels;

    for (int i = 0; i < numberOfModels; i++) {
        
        Model currModel = models[i];
        
        std::vector<int> modelVertices;
        
        //Find vertice ids used in the model. store them
        for (auto & triangle : currModel.triangles) {
            for (const auto vertexId : triangle.vertexIds) {
                
                if (std::find(modelVertices.begin(),modelVertices.end(),vertexId) == modelVertices.end()) {
                    modelVertices.push_back(vertexId);
                }
            } 
        }

        //model transform coordinates
        for (auto vertexId: modelVertices) {
            for (int j= 0; j < currModel.numberOfTransformations; j++) {
                
                char tType = currModel.transformationTypes[j];
                int tId = currModel.transformationIDs[j];
                
                tinymath::matrix transformMatrix;
    
                if (tType == 't') {
                    Translation t = translations[tId];
                    transformMatrix = tinymath::getTranslationMatrix(t);
                }
                else if (tType == 's') {
                    Scaling s = scalings[tId];
                    transformMatrix = tinymath::getScalingMatrix(s);
                }
                else { //currModel.type == 'r'
                    Rotation r = rotations[tId];
                    transformMatrix = tinymath::rotateAroundArbitraryAxis(r);
                }
                
                tinymath::vec4 vec = tinymath::vec4(vertices[vertexId]);
                vertices[vertexId] = tinymath::vec3(matrixMultVec4(vec, transformMatrix));
            }
        }
    }
}


void cameraTransform(Camera & cam) {
    tinymath::vec3 u = cam.u;
    tinymath::vec3 v = cam.v;
    tinymath::vec3 w = cam.w;

    tinymath::vec3 pos = cam.pos;
    
    tinymath::matrix cameraMatrix = tinymath::getMRotation(u,v,w);

    cameraMatrix.m[0][3] = -tinymath::dot(u,pos);
    cameraMatrix.m[1][3] = -tinymath::dot(v,pos);
    cameraMatrix.m[2][3] = -tinymath::dot(w,pos);


    for (int i=0; i < numberOfVertices; i++) {
        tinymath::vec4 vec = tinymath::vec4(vertices[i]);
        vertices[i] = tinymath::vec3(tinymath::matrixMultVec4(vec, cameraMatrix));
    } 
   
    tinymath::vec4 cVec = tinymath::vec4(cam.pos);
    cam.pos = tinymath::vec3(matrixMultVec4(cVec,cameraMatrix));
    
}

void perspectiveTransform(Camera cam) { //map to CVV
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

    perspectiveMatrix.m[0][0] = (2*n)/rminusl;
    perspectiveMatrix.m[0][2] = (r+l)/rminusl;
    perspectiveMatrix.m[1][1] = (2*n)/tminusb;
    perspectiveMatrix.m[1][2] = (t+b)/tminusb;
    perspectiveMatrix.m[2][2] = -(f+n)/fminusn;
    perspectiveMatrix.m[2][3] = (-2*f*n)/fminusn;
    perspectiveMatrix.m[3][2] = -1.0;
    perspectiveMatrix.m[3][3] = 0.0;

    for (int i=0; i < numberOfVertices; i++) {
        tinymath::vec4 vec = tinymath::vec4(vertices[i]);
        tinymath::vec4 res = tinymath::matrixMultVec4(vec, perspectiveMatrix);
        double t = res.t;
        vertices[i] = tinymath::vec3(res)/= t;
    } 
}

void viewportTransform(Camera cam) { //map to 2d
    int nx = cam.sizeX; 
    int ny = cam.sizeY; 
    
    tinymath::matrix viewportMatrix;
    
    viewportMatrix.m[0][0] = nx/2;
    viewportMatrix.m[0][3] = ny/2;
    viewportMatrix.m[1][1] = (nx-1)/2;
    viewportMatrix.m[1][3] = (ny-1)/2;
    viewportMatrix.m[2][2] = 0.5;
    viewportMatrix.m[2][3] = 0.5;


}

void rasterize() {} //TODO

void forwardRenderingPipeline(Camera cam) {
    modellingTransform();
    cameraTransform(cam); 
    perspectiveTransform(cam);
    viewportTransform(cam); //NOT IMPLEMENTED
    rasterize(); //NOT IMPLEMENTED
}

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;
    
    for (int i = 0; i < numberOfCameras; i++) {
        Camera currentCam = cameras[i];
        
        int nx = cameras[i].sizeX;
        int ny = cameras[i].sizeY;
        
        if (image) {
            for (int j = 0; j < cameras[i].sizeX; j++) {
                delete image[j];
            }
            delete[] image;
        }

        
        image = new Color*[nx];
        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image" << std::endl;
            exit(1);
        }

        for (int j = 0; j < nx; j ++) {
            image[j] = new Color[ny];

            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image" << std::endl;
                exit(1);
            }
        }
        
        
        initializeImage(currentCam);

        //OUR MAIN FUNCTION
        forwardRenderingPipeline(currentCam);

        writeImageToPPMFile(currentCam);

        convertPPMToPNG(currentCam.outputFileName, 1);
    
    }
    
    return 0;
}

