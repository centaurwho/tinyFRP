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


std::vector<tinymath::vec3> cameraTransform(const Camera & cam) {
    
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

std::vector<tinymath::vec3> 
perspectiveTransform(const Camera & cam, std::vector<tinymath::vec3> & relPositions) { //map to CVV

    double n = cam.n;
    double f = cam.f;
    double l = cam.l;
    double r = cam.r;
    double b = cam.b;
    double t = cam.t;

    for (auto & vertex: relPositions) {
        double x = vertex.x;
        double y = vertex.y;
        double z = vertex.z;

        vertex.x = -(2*n*x + (r+l)*z) / ((r-l)*z);
        vertex.y = -(2*n*y + (t+b)*z) / ((t-b)*z);
        vertex.z = (f+n)/(f-n) + (2*f*n)/(z*(f-n));
    }

    return relPositions;
}

void rasterizeLine(int x0, int y0, int x1, int y1, Color c0, Color c1) {

    bool l = false;

    if (abs(y1-y0) > abs(x1-x0)) {
        std::cout << "Long axis is y" << std::endl;
        std::cout << std::endl;
        l = true;
    }

    int inc;
    if ((l == false && y1 < y0) || (l == true && x1 < x0)) {
        inc = -1;
    } else {
        inc = 1;
    }

    std::cout << "Started rasterizing line" << std::endl;
    std::cout << "x0: " << x0 << " y0: " << y0 << std::endl;
    std::cout << "x1: " << x1 << " y1: " << y1 << std::endl;
    std::cout << std::endl;

    Color c = c0;
    
    if (l == true) {
        Color dc = (c1 - c0)/(y1 - y0);
    
        int x = x0;
        int d = inc*0.5*(y0 - y1) + (x1 - x0);

        for (int y = y0; y <= y1; y++) {
            image[x][y] = c.round();
            if (d*inc > 0) {
                x += inc;
                d += inc*(y0 - y1) + (x1 - x0); 
            }
            else 
                d+= (x1-x0);
            c = c+dc;
        }
        return;
    }
    
    int y = y0;
    int d = (y0 - y1) + inc*0.5*(x1 - x0);
    
    Color dc = (c1 - c0)/(x1 - x0);

    for (int x = x0; x <= x1; x++) {
        image[x][y] = c.round();
        if (d*inc < 0) {
            y += inc;
            d += (y0 - y1) + inc*(x1 - x0); 
        }
        else 
            d+= (y0-y1);
        c = c+dc;
    }
    std::cout << "Ended rasterizing line" << std::endl;
}



void viewportTransform(const Camera & cam, std::vector<tinymath::vec3> newPositions ) { //map to 2d
    int nx = cam.sizeX; 
    int ny = cam.sizeY; 
    
    for (const auto & model: models) {
        for (const auto & triangle: model.triangles) {
            
            //if (backfaceCullingSetting == 0 || 
            //    dot(calculateNormal(triangle), vertices[triangle.vertexIds[0]-1]) < 0) { 
            if (1) {  
                int xmin = nx, xmax = -1,
                    ymin = ny, ymax = -1;
                
                std::vector<tinymath::vec3> points;
                for (auto vertexId : triangle.vertexIds) {
                    tinymath::vec3 vertex = newPositions[vertexId-1];

                    int pixelx = (vertex.x*nx + (nx-1) + 1)/2;
                    int pixely = (vertex.y*ny + (ny-1) + 1)/2;
                    
                    vertex.x = pixelx;
                    vertex.y = pixely;

                    image[pixelx][pixely] = colors[vertex.colorId-1];
                    
                    points.push_back(vertex);
                    if (model.type == 1){
                        xmin = std::min(xmin,pixelx);
                        xmax = std::max(xmax,pixelx);
                        ymin = std::min(ymin,pixely);
                        ymax = std::max(ymax,pixely);
                    }
                }

                
                double x0 = points[0].x;
                int y0 = points[0].y;
                Color c0 = colors[points[0].colorId-1];
                int x1 = points[1].x;
                int y1 = points[1].y;
                Color c1 = colors[points[1].colorId-1];
                int x2 = points[2].x;
                int y2 = points[2].y;
                Color c2 = colors[points[2].colorId-1];

                line line01(x0,y0,x1,y1);
                double f01 = line01(x2,y2);
                line line12(x1,y1,x2,y2);
                double f12 = line12(x0,y0);
                line line20(x2,y2,x0,y0);
                double f20 = line20(x1,y1);

                //RASTERIZATION HERE
                if (model.type == 0) { //wireframe
                    rasterizeLine(x0,y0,x1,y1,c0,c1); 
                    rasterizeLine(x1,y1,x2,y2,c1,c2); 
                    rasterizeLine(x2,y2,x0,y0,c2,c0); 
                
                }
                else { //solid
                    for (int x = xmin; x <= xmax; x++){
                        for (int y = ymin; y <= ymax; y++){
                            double alpha = line12(x,y)/f12; 
                            double beta = line20(x,y)/f20; 
                            double gamma = line01(x,y)/f01; 
                            if (alpha >= 0 && beta >= 0 && gamma >= 0){
                                Color c;
                                c = c0*alpha + c1*beta + c2*gamma;
                                image[x][y] = c;
                            }
                        }
                    }             
                }
            }
        }
    } 

}

void forwardRenderingPipeline(const Camera & cam) {

    std::vector<tinymath::vec3> relPos = cameraTransform(cam); 
    perspectiveTransform(cam, relPos);
    viewportTransform(cam, relPos); 
}


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    modellingTransform();
    
    image = 0;
    for (auto & cam: cameras) {
         
        int nx = cam.sizeX;
        int ny = cam.sizeY;
        
        if (image) {
            for (int j = 0; j < nx; j++) {
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
        
        initializeImage(cam);

        //OUR MAIN FUNCTION
        forwardRenderingPipeline(cam);

        writeImageToPPMFile(cam);
        convertPPMToPNG(cam.outputFileName, 1);
}
    
    return 0;
}

