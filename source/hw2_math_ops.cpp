#include <cstdio>
#include <cmath>
#include "hw2_math_ops.h"
#define ABS(a) ((a)>0?(a):-1*(a))

/*
 * Makes matrix m (double[4][4]) an identity matrix (values on the diagonal are 1, others are 0).
*/
void makeIdentityMatrix(double m[4][4]) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (i == j) m[i][j] = 1.0;
            else m[i][j] = 0.0;
}

/*
 * Multiply matrices m1 (double[4][4]) and m2 (double[4][4]) and store the result in result matrix r (double[4][4]).
 */
void multiplyMatrixWithMatrix(double r[4][4], double m1[4][4], double m2[4][4]) {
    int i, j, k;
    double total;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            total = 0;
            for (k = 0; k < 4; k++)
                total += m1[i][k] * m2[k][j];
            r[i][j] = total;
        }
}

/*
 * Multiply matrix m (double[4][4]) with vector v (double[4]) and store the result in vector r (double[4]).
 */
void multiplyMatrixWithVec4d(double r[4], double m[4][4], double v[4]) {
    int i, j;
    double total;
    for (i = 0; i < 4; i++) {
        total = 0;
        for (j = 0; j < 4; j++)
            total += m[i][j] * v[j];
        r[i] = total;
    }
}
