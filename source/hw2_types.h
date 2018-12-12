#include "hw2_math_ops.h"
#include <unordered_set>

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

class Color {
public:
    double r, g, b;
    

    Color operator+(const Color & rhs) {
        Color c;
        c.r = r + rhs.r;
        c.g = g + rhs.g;
        c.b = b + rhs.b;
        return c;
    }
    
    Color operator-(const Color & rhs) {
        Color c;
        c.r = r - rhs.r;
        c.g = g - rhs.g;
        c.b = b - rhs.b;
        return c;
    }
    
    Color operator*(double d) {
        Color c;
        c.r = r*d;
        c.g = g*d;
        c.b = b*d;
        return c;
    }
    
    Color operator/(double d) {
        Color c;
        c.r = r/d;
        c.g = g/d;
        c.b = b/d;
        return c;
    }
    
    Color & round(){
        r = (int)(r + 0.5);
        g = (int)(g + 0.5);
        b = (int)(b + 0.5);
        return *this;
    }

    void swap(Color & rhs) {
        std::swap(r,rhs.r);
        std::swap(g,rhs.g);
        std::swap(b,rhs.b);
    }

    void print() {
        std::cout << r << " " << g << " " << b << std::endl;
    }

};

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
    std::vector<Triangle> triangles;

    std::unordered_set<int> usedVertices;
} Model;

class line {
public:
    int x0, x1, y0, y1;

    line(int x0, int y0, int x1, int y1)
        :x0(x0), y0(y0), x1(x1), y1(y1) {} 

    double operator()(int x, int y) const {
        return x0*y1-y0*x1 + x*(y0-y1) + y*(x1-x0);
    }
};

#endif //HW2_TYPES_H
