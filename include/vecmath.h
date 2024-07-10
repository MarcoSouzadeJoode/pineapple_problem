#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979

void vec_normalize(double *vector, int dim) {
    double magnitude = 0.0;

    for (int i = 0; i < dim; i++) {
        magnitude += vector[i]*vector[i];
    }
    magnitude = sqrt(magnitude);

    if (magnitude > 1e-15) {
        for (int i = 0; i<dim; i++) {
            vector[i] /= magnitude;
        }
    } else {
        printf("Zero division, mag=%lf\n",magnitude);
    }
}

void vec_to_polar(double *vector, double *r, double *phi, double *theta) {
    *r = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    *phi = atan2(vector[1], vector[0]);
    *theta = PI /2.0 - acos(vector[2] / (*r)); 
}


double vec_dot(double *vec1, double *vec2, int dim) {
    double res = 0;
    for (int i = 0; i < dim; i++) {
        res += vec1[i] * vec2[i];
    }
    return res;
}

void vec_add(double *vec1, double *vec2, int dim) {
    // add second into first vector
    for (int i = 0; i < dim; i++) {
        vec1[i] += vec2[i];
    }
}

void vec_multiply(double *vec, double alpha, int dim) {
    for (int i = 0; i < dim; i++) {
        vec[i] *= alpha;
    }
}

void vec_print(double *vector, int dim) {
    //printf("Vector: ");
    for (int i = 0; i< dim;i++) {
        printf("%lf\t",vector[i]);
    }
    printf("\n");
}