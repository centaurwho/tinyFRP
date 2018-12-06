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

void translate(vec3 & vec, const Translation & translation) {
    vec.x += translation.tx;
    vec.y += translation.ty;
    vec.z += translation.tz;
}

void rotate(vec3 & vec, const Rotation & rotation) {

    double u = rotation.ux;
    double v = rotation.uy;
    double w = rotation.uz;

    double x = vec.x;
    double y = vec.y;
    double z = vec.z;

    double cosT = cos(RADIANS(rotation.angle));
    double minusCos = 1 - cosT; 
    double sinT = sin(RADIANS(rotation.angle));

    double rep = (u*x+v*y+w*z)*minusCos;

    vec.x = u*rep + x*cosT + (-w*y + v*z)*sinT;
    vec.y = v*rep + y*cosT + (w*x - u*z)*sinT;
    vec.z = w*rep + z*cosT + (-v*x + u*y)*sinT;

}

void scale(vec3 & vec, const Scaling & scaling) {
      
    vec.x *= scaling.sx;
    vec.y *= scaling.sy;
    vec.z *= scaling.sz;
}
}
