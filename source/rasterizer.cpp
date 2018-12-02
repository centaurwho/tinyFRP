#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>


#include "hw2_types.h"
#include "hw2_file_ops.h"
#include "tinymath.h"


std::vector<Camera> cameras;
std::vector<Model> models;
std::vector<Color> colors;
std::vector<Translation> translations;
std::vector<Rotation> rotations;
std::vector<Scaling> scalings;
std::vector<tinymath::vec3> vertices;

Color backgroundColor;

extern int numberOfVertices;

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

tinymath::vec3 calculateNormal(const Triangle & triangle) {

    tinymath::vec3 normal;
    
    tinymath::vec3 vertexA = vertices[triangle.vertexIds[0]-1];
    tinymath::vec3 vertexB = vertices[triangle.vertexIds[1]-1];
    tinymath::vec3 vertexC = vertices[triangle.vertexIds[2]-1];

    tinymath::vec3 edgeBC = vertexC - vertexB;
    tinymath::vec3 edgeBA = vertexA - vertexB;

    normal = tinymath::normalize(tinymath::cross(edgeBC, edgeBA));
    return normal;
}


void modellingTransform() { 
    std::vector<tinymath::vec3> transModels;
    
    for (const auto & model: models) {
        
        for (auto vertexId: model.usedVertices) {
            for (int i= 0; i < model.numberOfTransformations; i++) {
                
                char tType = model.transformationTypes[i];
                int tId = model.transformationIDs[i];
   
                if (tType == 't') {
                    Translation t = translations[tId-1];
                    tinymath::translate(vertices[vertexId-1],t);
                }
                else if (tType == 's') {
                    Scaling s = scalings[tId-1];
                    tinymath::scale(vertices[vertexId-1],s);
                }
                else { //model.type == 'r'
                    Rotation r = rotations[tId-1];
                    tinymath::rotate(vertices[vertexId-1],r);
                }
            }
        }
    }
}


std::vector<tinymath::vec3> cameraTransform(Camera & cam) {
    
    tinymath::vec3 u = cam.u;
    tinymath::vec3 v = cam.v;
    tinymath::vec3 w = cam.w;
    tinymath::vec3 pos = cam.pos;
    
    std::vector<tinymath::vec3> newPositions;
    
    for (const auto & vertex: vertices) {
        tinymath::vec3 vec = vertex;
        
        vec.x = dot(vertex,u) - dot(pos,u);
        vec.y = dot(vertex,v) - dot(pos,v);
        vec.z = dot(vertex,w) - dot(pos,w);

        newPositions.push_back(vec);
    }
    
    return newPositions;
}

std::vector<tinymath::vec3> perspectiveTransform(Camera cam, std::vector<tinymath::vec3> newPositions) { //map to CVV

    double n = cam.n;
    double f = cam.f;
    double l = cam.l;
    double r = cam.r;
    double b = cam.b;
    double t = cam.t;

    for (auto & vertex: newPositions) {
        double x = vertex.x;
        double y = vertex.y;
        double z = vertex.z;

        vertex.x = -(2*n*x + (r+l)*z) / ((r-l)*z);
        vertex.y = -(2*n*y + (t+b)*z) / ((t-b)*z);
        vertex.z = (f+n)/(f-n) + (2*f*n)/(z*(f-n));
    }

    return newPositions;
}

void viewportTransform(Camera cam, std::vector<tinymath::vec3> newPositions ) { //map to 2d
    int nx = cam.sizeX; 
    int ny = cam.sizeY; 

    for (const auto & model: models) {
        for (const auto & triangle: model.triangles) {

            if (backfaceCullingSetting == 0 || dot(calculateNormal(triangle), vertices[triangle.vertexIds[0]]) < 0) { 
                for (auto vertexId : triangle.vertexIds) {
                    tinymath::vec3 vertex = newPositions[vertexId-1];

                    double x = vertex.x; 
                    double y = vertex.y;
                    double z = vertex.z;

                    Color color = colors[vertex.colorId -1];

                    int pixelx = (int)((x*nx + (nx-1) + 1)/2);
                    int pixely = (int)((y*ny + (ny-1) + 1)/2);
                    
                    image[pixelx][pixely] = color;
                }
            }
        }
    } 

}

void rasterize() {} //TODO

void forwardRenderingPipeline(Camera cam) {
   
    modellingTransform();
    
    std::vector<tinymath::vec3> camPos = cameraTransform(cam); 
    std::vector<tinymath::vec3> perPos = perspectiveTransform(cam,camPos);
    viewportTransform(cam, perPos); 
    
    //rasterize(); //NOT IMPLEMENTED
}


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (int i = 0; i < 1; i++) {
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

