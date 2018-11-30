#include "tinymath.h"

#include <cstdio>
#include <iostream>

Translation::Translation(double tx, double ty, double tz)
    :tx(tx), ty(ty), tz(tz) {}

Translation::Translation(const tinymath::vec3 & vec)
    :tx(vec.x), ty(vec.y), tz(vec.z) {}

Scaling::Scaling(double sx, double sy, double sz)
    :sx(sx), sy(sy), sz(sz) {}

Scaling::Scaling(const tinymath::vec3 & vec)
    :sx(vec.x), sy(vec.y), sz(vec.z) {}

Rotation::Rotation(double angle, double ux, double uy, double uz)
    :angle(angle), ux(ux), uy(uy), uz(uz) {}

Rotation::Rotation(const tinymath::vec3 & vec)
    :ux(vec.x), uy(vec.y), uz(vec.z) {}

namespace tinymath {

vec3::vec3(double x, double y, double z, int colorId)
    : x(x), y(y), z(z), colorId(colorId) { }

vec3::vec3(const vec4 & vec)
    : x(vec.x),y(vec.y),z(vec.z) {}

vec3 & vec3::operator+=(const vec3 & rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;

    return *this;
}

bool operator==(const vec3 & lhs, const vec3 & rhs) {
    const double epsilon = 1e-6;
    return std::abs(lhs.x - rhs.x) < epsilon
        && std::abs(lhs.y - rhs.y) < epsilon
        && std::abs(lhs.z - rhs.z) < epsilon;

}

bool operator!=(const vec3 & lhs, const vec3 & rhs) {
    return !(lhs == rhs);
}

vec3 operator+(vec3 lhs, const vec3 & rhs) {
    lhs += rhs;
    return lhs;
}

vec3 & vec3::operator-=(const vec3 & rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;

    return *this;
}

vec3 operator-(vec3 lhs, const vec3 & rhs) {
    lhs -= rhs;
    return lhs;
}

vec3 & vec3::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

vec3 operator*(vec3 lhs, double scalar) {
    lhs *= scalar;
    return lhs;
}

vec3 operator*(double scalar, vec3 rhs) {
    rhs *= scalar;
    return rhs;
}

vec3 & vec3::operator/=(double scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

vec3 operator/(vec3 lhs, double scalar) {
    lhs /= scalar;
    return lhs;
}

double length(const vec3 & vec) {
    double result = std::sqrt( vec.x * vec.x
                            + vec.y * vec.y
                            + vec.z * vec.z );

    return result;
}

vec3 normalize(vec3 vec) {
    double vecLength = length(vec);
    if (vecLength == 0.0f)
        return vec3(); // return zero vector

    vec /= vecLength;
    return vec;
}

double dot(const vec3 & lhs, const vec3 & rhs) {
    return lhs.x * rhs.x
         + lhs.y * rhs.y
         + lhs.z * rhs.z;
}

vec3 cross(const vec3 & lhs, const vec3 & rhs) {
    vec3 result;
    result.x = lhs.y * rhs.z - lhs.z * rhs.y;
    result.y = lhs.z * rhs.x - lhs.x * rhs.z;
    result.z = lhs.x * rhs.y - lhs.y * rhs.x;
    return result;
}

void printVec3(const vec3 & vec) {
    printf("(%lf,%lf,%lf)\n", vec.x, vec.y, vec.z);
}

//vec4 below

vec4::vec4(double x, double y, double z, double t)
    : x(x), y(y), z(z), t(t) { }

vec4::vec4(const vec3 & vertice)
    : x(vertice.x), y(vertice.y), z(vertice.z), t(1) { }
    
double & vec4::operator[](const int index){

    switch (index) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        default:
            return this->t;
    }
}


void printVec4(const vec4 & vec) {
    printf("(%lf,%lf,%lf,%lf)\n", vec.x, vec.y, vec.z, vec.t);
}

matrix::matrix(int size) {
    this->size = size;
    for (int i=0; i<size; i++) {    
        std::vector<double> temp;
        for (int j=0; j<size; j++) {
            temp.push_back(0.0);
        }
        m.push_back(temp);
    }
    m[size-1][size-1] = 1.0;
}

matrix::matrix(int size, std::vector<std::vector<double>> mat) {
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            m[i][j] = mat[i][j];
        }
    }
}

matrix & makeTranspose(matrix & mat) {
    for (int i = 0; i < mat.size; i++) {
        for (int j=0; j < i; j++) {
            double temp = mat.m[i][j];
            mat.m[i][j] = mat.m[j][i];
            mat.m[j][i] = temp;
        }
    } 
    return mat;
}


matrix & makeIdentity(matrix & mat) {
    for (int i = 0; i < mat.size; i++){
        for (int j = 0; j < mat.size; j++) {
            mat.m[i][j] = i==j ? 1.0 : 0.0;
        }
    }

    return mat;
}


matrix getTranslationMatrix(const Translation & translation) {

    matrix mat = matrix();
    makeIdentity(mat);

    mat.m[0][3] = translation.tx;
    mat.m[1][3] = translation.ty;
    mat.m[2][3] = translation.tz;

    return mat; 
}


matrix getRotationMatrixAroundX(double angle) {
    
    matrix mat = matrix();

    mat.m[1][1] = cos(RADIANS(angle));
    mat.m[1][2] = -sin(RADIANS(angle));
    mat.m[2][1] = sin(RADIANS(angle));
    mat.m[2][2] = cos(RADIANS(angle));

    return mat; 
}

matrix rotateAroundArbitraryAxis(const Rotation & rotation) {
    
    //TODO: This code is messy. Remove redundant stuff
    Translation tOrig = Translation(vec3(-rotation.ux, -rotation.uy, -rotation.uz));
    matrix translationToOrigin = getTranslationMatrix(tOrig);

    vec3 u = vec3(rotation.ux, rotation.uy, rotation.uz); //u is also rotation axis
    Translation tPoint = Translation(vec3(u));
    matrix translationToPoint = getTranslationMatrix(tPoint);

    normalize(u);
    printVec3(u);

    vec3 v = vec3(-u.y, u.x, 0);
    vec3 w = vec3(cross(u,v));
    normalize(v);
    normalize(w);
    
    matrix rotateAroundX = getRotationMatrixAroundX(rotation.angle);
    
    matrix m = getMRotation(u,v,w);
    matrix mTranspose = matrix(makeTranspose(m));

    //TODO: Handle this mess
    matrix res1 = matrix(matrixMultMatrix(translationToPoint,mTranspose));
    matrix res2 = matrix(matrixMultMatrix(res1,rotateAroundX));
    matrix res3 = matrix(matrixMultMatrix(res2,m));
    matrix res4 = matrix(matrixMultMatrix(res3,translationToOrigin));
    
    return res4;
}

matrix getMRotation(const vec3 & u, const vec3 & v, const vec3 & w) {
    matrix mat = matrix();

    mat.m[0][0] = u.x;
    mat.m[0][1] = u.y;
    mat.m[0][2] = u.z;
    mat.m[1][0] = v.x;
    mat.m[1][1] = v.y;
    mat.m[1][2] = v.z;
    mat.m[2][0] = w.x;
    mat.m[2][1] = w.y;
    mat.m[2][2] = w.z;

    return mat;
}

matrix getScalingMatrix(const Scaling & scaling) {
      
    matrix mat = matrix();

    mat.m[0][0] = scaling.sx;
    mat.m[1][1] = scaling.sy;
    mat.m[2][2] = scaling.sz;

    return mat;
}

matrix matrixMultMatrix(const matrix & mat1, const matrix & mat2) {
    if (mat1.size != mat2.size)
        std::cout << "Matrix sizes does not match" << std::endl;
    double total;
    
    int size = mat1.size;
    matrix res = matrix(size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            total = 0;
            for (int k = 0; k < size; k++) {          
                total += mat1.m[i][k] * mat2.m[k][j];
            }
            res.m[i][j] = total;
        }
    }
    return res;
}

vec4 matrixMultVec4(vec4 & vec, const matrix & mat){
    if (mat.size != 4){
        std::cout << "Matrix size does not match vector size" << std::endl;
    }

    vec4 res;
    double total;
    int size = 4;
    for (int i = 0; i < size ; i++) {
        res[i] = 0;
        for (int j = 0; j < size; j++) {
            res[i] += mat.m[i][j]*vec[j]; 
        }
    }
    return res;
}

void printMatrix(const matrix & mat) {
    for (int i = 0; i < mat.size; i++) {
        std::cout << "[";
        for (int j = 0; j < mat.size; j++) {
            std::cout << mat.m[i][j] << " ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;
}

}
