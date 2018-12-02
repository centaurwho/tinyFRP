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

tinymath::vec3 calculateNormal(Triangle & triangle) {

    tinymath::vec3 normal;
    
    tinymath::vec3 vertexA = vertices[triangle.vertexIds[0]];
    tinymath::vec3 vertexB = vertices[triangle.vertexIds[1]];
    tinymath::vec3 vertexC = vertices[triangle.vertexIds[2]];

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
                    Translation t = translations[tId];
                    tinymath::translate(vertices[vertexId-1],t);
                }
                else if (tType == 's') {
                    Scaling s = scalings[tId];
                    tinymath::scale(vertices[vertexId-1],s);
                }
                else { //model.type == 'r'
                    Rotation r = rotations[tId];
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
    tinymath::printVec3(pos);
    
 
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

    //for (int i=0; i < numberOfVertices; i++) {
    //    int colorId = vertices[i].colorId;
    //    tinymath::vec4 vec = tinymath::vec4(vertices[i]);
    //    tinymath::vec4 res = tinymath::matrixMultVec4(vec, perspectiveMatrix);
    //    double t = res.t;
    //    vertices[i] = tinymath::vec3(res)/= t;
    //    vertices[i].colorId = colorId;
    //} 
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
//
//    for (int i = 0; i < numberOfModels; i++) {
//
//        Model model = models[i];
//        for (auto triangle: model.triangles) {
//
//            if (backfaceCullingSetting == 0 || dot(calculateNormal(triangle), vertices[triangle.vertexIds[0]]) < 0) { 
//            
//                for (auto vertexId : triangle.vertexIds) {
//                    
//                    tinymath::vec4 v = tinymath::vec4(vertices[vertexId-1]);
//                    tinymath::vec4 vec = matrixMultVec4(v, viewportMatrix);
//                    tinymath::vec3 res = tinymath::vec3(vec);
//                    int x = (int) (res.x + 0.5); 
//                    int y = (int) (res.y + 0.5); 
//                    image[x][y] = colors[vertices[vertexId].colorId-1];
//                }
//            }
//        }
//    } 

}

void rasterize() {} //TODO

void forwardRenderingPipeline(Camera cam) {
    
    modellingTransform();

    cameraTransform(cam); 
    //perspectiveTransform(cam);
    //viewportTransform(cam); //NOT IMPLEMENTED
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

