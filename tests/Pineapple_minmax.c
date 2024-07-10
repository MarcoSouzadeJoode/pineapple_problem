#include <stdio.h>
#include <math.h>
#include <time.h>


// TODO: vyzkoušet polární den a polární noc!
// chytřejší zadávání ročních období, ... t0 jako input

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
    double epsilon = 23 * 3.1415926535 / 180.0;
    double OMG = 6.263185 / 365.25 / 86400;
    omega *= -1.0;

    rsun[0] = cos(OMG*t)*cos(omega*t) + sin(OMG*t)*sin(omega*t)*cos(epsilon);
    rsun[1] = cos(OMG*t)*sin(omega*t) - sin(OMG*t)*cos(omega*t)*cos(epsilon);
    rsun[2] =-sin(OMG*t)*sin(epsilon);
}

void Rsun_simple(double *rsun, double t, double omega, int dim){
    rsun[0] = cos(omega * t);
    rsun[1] = -sin(omega * t) ;
    rsun[2] =0;
}



void iteration(double t, double omega, double *rsun, double *vector, double cos_theta, int dim) {
    double alpha, beta, cos_z;
    
    // cos_theta codes the velocity of motion on the planet
    Rsun_inc(rsun, t, omega, dim);


    cos_z = vec_dot(rsun, vector, dim);

    if (cos_z > 0) {

    // the equations allow for a negative beta solution
    // which is not yet tested / implemented
    // however, the positive solution seems reasonable!

    beta =  sqrt( (1-cos_theta*cos_theta) / (1-cos_z*cos_z) );
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
 double *vector, double cos_theta, int dim, const char *filename,
    double *minlat, double *maxlat) {
    double t = 0;
    double r, theta, phi, thetas, phis;
    minlat = 0;
    maxlat = 0;

    FILE *file = fopen(filename, "w");
    //fprintf(file, "r\ttheta\tphi\n");
    fprintf(file, "t\ttheta\tphi\tthetas\tphis\n");

    for (int j = 0; j < N; j++) {
        r = 0;
        theta = 0;
        phi = 0;
        thetas = 0;
        phis = 0;

        iteration(t, omega, rsun, vector, cos_theta, dim);
        //vec_print(vector, dim);

        vec_to_polar(vector, &r, &phi, &theta);
        vec_to_polar(rsun, &r, &phis, &thetas);

        if (*minlat > theta) {*minlat = theta;}
        if (*maxlat < theta) {*maxlat = theta;}

        fprintf(file, "%lf\t%lf\t%lf\t%lf\t%lf\n", t, theta, phi, thetas, phis);
        
        //fprintf(file, "%lf\t%lf\t%lf\n", vector[0], vector[1],vector[2]);
        
        t += dt;
    }

    fclose(file);
}



int single_run(double T) {
    clock_t begin = clock();
    char filename[64];
    sprintf(filename, "days/T_%lf.txt",T);
    //printf(filename);
    int N = 15768000 / 5; // iterations -> 5 years
    double dt = 10; // seconds
    double minlat, maxlat;
    double vector[] = {0.2,0.0,1}; // initial position
    double rsun[] = {1.0,0.0,0.0}; // dummy vector
    int dim = sizeof(vector) / sizeof(vector[0]);




    double cos_theta, R_planet, omega, v;
    
    minlat = 0.0;
    maxlat = 0.0;

    R_planet = 6.378e6; // m
    v = 10; // m/s
    cos_theta =  cos(v * dt / R_planet);
    //T = 86400; // s, Earth
    omega = 3.14159265358979 * 2 / T;

    vec_normalize(vector, dim);
    vec_print(vector, dim);
    
    looper(N, dt, omega, rsun, vector, cos_theta, dim, filename, &minlat, &maxlat);

    printf("MINLAT: %lf \n MAXLAT: %lf", minlat, maxlat);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("DONE. Iter: %lf, Time: %lf\n",T, time_spent);
    return 0;
}

int main() {
    double T;
    for (int i = 50000; i < 100000; i += 50000) {
        printf("%d",i);
        T = i * 1.0 + 1e-15;
        single_run(T);
    }
}