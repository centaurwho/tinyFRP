#include "hw2_types.h"

#ifndef HW2_MATH_OPS_H
#define HW2_MATH_OPS_H


/*
 * Makes matrix m (double[4][4]) an identity matrix (values on the diagonal are 1, others are 0).
*/
void makeIdentityMatrix(double m[4][4]);

/*
 * Multiply matrices m1 (double[4][4]) and m2 (double[4][4]) and store the result in result matrix r (double[4][4]).
 */
void multiplyMatrixWithMatrix(double r[4][4], double m1[4][4], double m2[4][4]);

/*
 * Multiply matrix m (double[4][4]) with vector v (double[4]) and store the result in vector r (double[4]).
 */
void multiplyMatrixWithVec4d(double r[4], double m[4][4], double v[4]);


#endif //HW2_MATH_OPS_H
