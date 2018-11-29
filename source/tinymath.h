#include <vector>
#define ABS(a) ((a)>0?(a):-1*(a))

#ifndef TINYMATH_H
#define TINYMATH_H


namespace tinymath {


class vec3 {

public:
    double x;
    double y;
    double z;
    
    int colorId;

    explicit vec3(double x = 0, double y = 0, double z = 0, int colorId = -1);    
    vec3(const vec3 &) = default;
    vec3 & operator=(const vec3 &) = default;

    // equality
    friend bool operator==(const vec3 & lhs, const vec3 & rhs);
    friend bool operator!=(const vec3 & lhs, const vec3 & rhs);

    // addition
    vec3 & operator+=(const vec3 & rhs);
    friend vec3 operator+(vec3 lhs, const vec3 & rhs);

    // substraction
    vec3 & operator-=(const vec3 & rhs); 
    friend vec3 operator-(vec3 lhs, const vec3 & rhs); 

    // scalar multiplication
    vec3 & operator*=(double scalar);
    friend vec3 operator*(vec3 lhs, double scalar);
    friend vec3 operator*(double scalar, vec3 rhs);

    vec3 & operator/=(double scalar);
    friend vec3 operator/(vec3 lhs, double scalar);
};

double length(const vec3 & vec);
vec3 normalize(vec3 vec);
double dot(const vec3 & lhs, const vec3 & rhs);
vec3 cross(const vec3 & lhs, const vec3 & rhs);
void printVec3(const vec3 & vec);

class vec4 {

public:
    double x;
    double y;
    double z;
    double t;

    explicit vec4(double x = 0, double y = 0, double z = 0, double t = 1);  
    explicit vec4(const vec3 &);  
    vec4(const vec4 &) = default;
    vec4 & operator=(const vec4 &) = default;
};

void printVec4(const vec4 & vec);

class matrix {

public:
    int size;
    std::vector<std::vector<double>> m;
    
    matrix(int size);
    matrix(int size, std::vector<std::vector<double>> matrix);

}; 

matrix makeIdentity(matrix m);
matrix operator*(const matrix & lhs,const matrix & rhs);
vec4 operator*(const vec4 & lhs, const vec4 & rhs);
void printMatrix(const matrix & m);

} // namespace tinymath

#endif /* TINYMATH_H */

