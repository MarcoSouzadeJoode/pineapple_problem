#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979
#define dim 3

struct vector {
    double x, y, z;
};

struct quaternion {
    double w, x, y, z;
};


struct vector vec_cross(struct vector *v1, struct vector *v2) {
    struct vector V;
    V.x = v1->y * v2->z - v2->y * v1->z;
    V.y = v1->z * v2->x - v2->z * v1->x;
    V.z = v1->x * v2->y - v2->x * v1->y;
    return V;
};

struct vector vec_from_quat(struct quaternion *q) {
    struct vector V;
    V.x = q->x;
    V.y = q->y;
    V.z = q->z;
    return V;
};

double q_norm(struct quaternion *q) {
    double norm = sqrt(q->w * q->w + q->x*q->x + q->y * q->y + q->z * q->z);
    return norm;
};

double q_norm_sq(struct quaternion *q) {
    return q->w * q->w + q->x*q->x + q->y * q->y + q->z * q->z;
};


void q_to_polar(struct quaternion *q, double *r, double *phi, double *theta) {
    *r = sqrt(q->x * q->x + q->y * q->y + q->z * q->z);
    *phi = atan2(q->y, q->x);
    *theta = PI /2.0 - acos(q->z/ (*r)); 
}

void q_normalize (struct quaternion *q) {
    double norm = q_norm(q);
    q->w /= norm;
    q->x /= norm;
    q->y /= norm;
    q->z /= norm;
};

struct quaternion q_inverse(struct quaternion *q) {
    struct quaternion Q;
    double norm_sq = q_norm_sq(q);
    Q.w = +q->w / norm_sq;
    Q.x = -q->x / norm_sq;
    Q.y = -q->y / norm_sq;
    Q.z = -q->z / norm_sq;
    return Q;
};

struct quaternion q_add(struct quaternion *q1, struct quaternion *q2) {
    struct quaternion Q;
    Q.w = q1->w + q2->w;
    Q.x = q1->x + q2->x;
    Q.y = q1->y + q2->y;
    Q.z = q1->z + q2->z;
    return Q;
};

struct quaternion q_mult(struct quaternion *q1, struct quaternion *q2) {
    struct quaternion Q;
    Q.w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y -q1->z * q2->z;
    Q.x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z -q1->z * q2->y;
    Q.y = q1->w * q2->y - q1->x * q2->z + q1->y * q2->w +q1->z * q2->x;
    Q.z = q1->w * q2->z + q1->x * q2->y - q1->y * q2->x +q1->z * q2->w;
    return Q;
};

struct quaternion q_versor(struct vector *v, double theta) {
    struct quaternion Q;
    double sin_theta_half = sin(theta) / 2;
    Q.w = cos(theta / 2);
    Q.x = v->x * sin_theta_half;
    Q.y = v->y * sin_theta_half;
    Q.z = v->z * sin_theta_half;
    return Q;
};

