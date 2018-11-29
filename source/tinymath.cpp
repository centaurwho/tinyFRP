#include "tinymath.h"

#include <cmath>
#include <cstdio>

namespace tinymath {

vec3::vec3(double x, double y, double z, int colorId)
    : x(x), y(y), z(z), colorId(colorId) { }

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


void printVec4(const vec4 & vec) {
    printf("(%lf,%lf,%lf,%lf)\n", vec.x, vec.y, vec.z, vec.t);
}

matrix::matrix(int size) {
    for (int i=0; i<size; i++) {    
        std::vector<double> temp;
        for (int j=0; j<size; j++) {
            temp.push_back(0);
        }
        m.push_back(temp);
    }
}

matrix::matrix(int size, std::vector<std::vector<double>> matrix) {
    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            m[i][j] = matrix[i][j];
        }
    }
}

matrix makeIdentity(matrix mat) {
    for (int i = 0; i < mat.size; i++){
        for (int j = 0; j < mat.size; j++) {
            mat.m[i][j] = i==j ? 1.0 : 0.0;
        }
    }
}

} // namespace tinymath

