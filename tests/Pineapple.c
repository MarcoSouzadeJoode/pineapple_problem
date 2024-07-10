#include <stdio.h>
#include <math.h>
#include <time.h>

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
    *theta = 3.14159265358979/2.0 - acos(vector[2] / (*r)); 
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
    double res = 0;
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

void Rsun_inc(double *rsun, double t, double omega, int dim){
    //  TODO upravit na pohyb se sklonem
    // klíčové slovo: https://en.wikipedia.org/wiki/Euler_angles
    rsun[0] = cos(omega * t);
    rsun[1] = -sin(omega * t) ;
    rsun[2] =0;
}

void Rsun_simple(double *rsun, double t, double omega, int dim){
    rsun[0] = cos(omega * t);
    rsun[1] = -sin(omega * t) ;
    rsun[2] =0;
}



void iteration(double t, double omega, double *rsun, double *vector, double cos_theta, int dim) {
    double alpha, beta, cos_z;
    // cos_theta codes the velocity of motion on the planet
    Rsun_simple(rsun, t, omega, dim);

    cos_z = vec_dot(rsun, vector, dim);

    if (cos_z > 0) {

    // the equations allow for a negative beta solution
    // which is not yet tested / implemented
    // however, the positive solution seems reasonable!
    beta = sqrt( (1-cos_theta*cos_theta) / (1-cos_z*cos_z) );
    alpha = cos_theta - cos_z * beta;


    vec_multiply(vector, alpha, dim);

    // BEWARE: rsun modified here, needs to be recalculated in next steps again!
    vec_multiply(rsun, beta, dim);
    vec_add(vector, rsun, dim);
    } else {
        //printf("WALKER SLEEPING\n");
    }
}

void looper(int N, double dt, double omega, double *rsun,
 double *vector, double cos_theta, int dim, const char *filename) {
    double t = 0;
    double r, theta, phi;

    FILE *file = fopen(filename, "w");
    //fprintf(file, "r\ttheta\tphi\n");
    fprintf(file, "t\ttheta\tphi\n");

    for (int j = 0; j < N; j++) {
        r = 0;
        theta = 0;
        phi = 0;

        iteration(t, omega, rsun, vector, cos_theta, dim);
        //vec_print(vector, dim);

        vec_to_polar(vector, &r, &phi, &theta);
        fprintf(file, "%lf\t%lf\t%lf\n", t, theta, phi);
        
        //fprintf(file, "%lf\t%lf\t%lf\n", vector[0], vector[1],vector[2]);
        
        t += dt;
    }

    fclose(file);
}



int main() {
    clock_t begin = clock();


    int N = 1000000; // iterations
    double dt = 1; // seconds

    double vector[] = {0.01,0.0,1}; // initial position
    double rsun[] = {1.0,0.0,0.0}; // dummy vector
    int dim = sizeof(vector) / sizeof(vector[0]);


    double cos_theta, R_planet, v, omega, T;
    

    R_planet = 6.378e6; // m
    v = 10; // m/s
    cos_theta =  cos(v * dt / R_planet);
    T = 86400; // s, Earth
    omega = 3.14159265358979 * 2 / T;

    vec_normalize(vector, dim);
    vec_print(vector, dim);
    
    looper(N, dt, omega, rsun, vector, cos_theta, dim, "P/P3.txt");
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("DONE. Time: %lf\n",time_spent);
    return 0;
}