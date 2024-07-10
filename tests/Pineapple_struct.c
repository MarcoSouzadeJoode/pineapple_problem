#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "vecmath.h"

#define PI 3.14159265358979
#define dim 3

struct walker {
    double vector[dim];
    double r;
    double theta;
    double lambda;
    double v;
    double cos_theta;
};

struct planet {
    double rsun[dim];
    double radius;
    double rs;
    double theta_s;
    double lambda_s;
    double omega;
    double orb_omega;
    double theta_0;
    double epsilon; // obliquity
    double oblateness; //non-sphericity
};

struct simulation_parameters {
    long N;
    double t;
    double dt;
    double cos_z;
    double alpha;
    double beta;
    char filename[64];
};

void sim_init(struct walker *w, struct planet *p, struct simulation_parameters *sim);

void Rsun_inc(double *rsun, double t, double omega, double theta_0){
    // klíčové slovo: https://en.wikipedia.org/wiki/Euler_angles
    double epsilon = 23.5 * PI / 180.0;
    double OMG = 6.263185 / 365.25 / 86400;
    rsun[0] = cos(OMG*t + theta_0)*cos(omega*t) + sin(OMG*t + theta_0)*sin(omega*t)*cos(epsilon);
    rsun[1] = cos(OMG*t + theta_0)*sin(omega*t) - sin(OMG*t + theta_0)*cos(omega*t)*cos(epsilon);
    rsun[2] =-sin(OMG*t + theta_0)*sin(epsilon);
}

void Rsun_simple(double *rsun, double t, double omega){
    rsun[0] = cos(omega * t);
    rsun[1] = -sin(omega * t) ;
    rsun[2] =0;
}

void simulation_step(struct walker *w, struct planet *p,
 struct simulation_parameters *sim){
    // for spherical earth
    //double alpha, beta;
    Rsun_inc(p->rsun, sim->t, p->omega, p->theta_0);
    sim->cos_z = vec_dot(p->rsun, w->vector, dim);
    if (sim->cos_z  > 0) {
        sim->beta = sqrt( (1-w->cos_theta*w->cos_theta) / (1-sim->cos_z*sim->cos_z) );
        sim->alpha = w->cos_theta - sim->cos_z * sim->beta;
        vec_multiply(w->vector, sim->alpha, dim);
        vec_multiply(p->rsun, sim->beta, dim);
        vec_add(w->vector, p->rsun, dim);
    }
}

void print_to_file(struct walker *w, struct planet *p,
 struct simulation_parameters *sim, FILE *file) {
    fprintf(file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", 
    sim->t, w->r, w->theta, w->lambda,
    w->vector[0], w->vector[1],w->vector[2],
    p->rsun[0],p->rsun[1],p->rsun[2]);

};

void print_header(struct walker *w, struct planet *p,
struct simulation_parameters *sim, FILE *file, FILE *metadata) {

    // header for data file
    fprintf(file, "t\tr\ttheta\tlambda\tx\ty\tz\txs\tys\tzs\n");

    //create metadata file

    fprintf(metadata, "INITIAL WALKER PARAMETERS\n");
    fprintf(metadata, "THETA\t%lf\n", w->theta);
    fprintf(metadata, "LAMBDA\t%lf\n", w->lambda);
    fprintf(metadata, "VELOCITY\t%lf\n\n", w->v);

    fprintf(metadata, "ASTRONOMY PARAMETERS\n");
    fprintf(metadata, "OMEGA\t%lf\n",p->omega);
    fprintf(metadata, "OMEGA_ORB\t%lf\n",p->orb_omega);
    fprintf(metadata, "INITIAL_ANOMALY\t%lf\n",p->theta_0);
    fprintf(metadata, "EPSILON\t%lf\n\n",p->epsilon);

    fprintf(metadata, "SIMULATION PARAMETERS\n");
    fprintf(metadata, "SIMULATION_STEPS\t%ld\n",sim->N);
    fprintf(metadata, "TIME_STEP\t%lf\n",sim->dt);

}

void simulation(struct walker *w, struct planet *p, struct simulation_parameters *sim) {
    FILE *file = fopen(sim->filename, "w");
    FILE *metadata = fopen(strcat(sim->filename, "_metadata"), "w");
    print_header(w, p, sim, file, metadata);
    fclose(metadata);

    for (int i = 0; i < sim->N; i++) {
        vec_to_polar(w->vector, &(w->r), &(w->lambda), &(w->theta));
        vec_to_polar(p->rsun, &(p->rs), &(p->lambda_s), &(p->theta_s));
        simulation_step(w, p, sim);
        print_to_file(w, p, sim, file); //problém s konst?
        sim->t += sim->dt;
    }

    fclose(file);
}

void parameter_scan(struct walker *w, struct planet *p, struct simulation_parameters *sim) {
    
};


int main() {
    struct planet p;
    struct walker w;
    struct simulation_parameters sim;

    sim_init(&w, &p, &sim);
    simulation(&w, &p, &sim);

    return 0;
}



void sim_init(struct walker *w, struct planet *p, struct simulation_parameters *sim) {
    w->vector[0] = 0.5;
    w->vector[1] = 0;
    w->vector[2] = 1.0;
    w->v = 5; // m/s

    p->radius = 6e6;
    p->omega = 2*PI / 86146;
    p->orb_omega = p->omega / 365.25;
    p->epsilon = 23.5 * PI / 180.0;

    p->rsun[0] = 1.0;
    p->rsun[1] = 0;
    p->rsun[2] = 0;

    vec_normalize(w->vector,dim);
    vec_normalize(p->rsun,dim);

    sim->N = 10000000;
    sim->t = 0;
    sim->dt = 10; // s

    strncpy(sim->filename, "test_A", sizeof(sim->filename) - 1);
    sim->filename[sizeof(sim->filename) - 1] = '\0';

    vec_to_polar(w->vector, &(w->r), &(w->lambda), &(w->theta));
    vec_to_polar(p->rsun, &(p->rs), &(p->lambda_s), &(p->theta_s));

    sim->cos_z = vec_dot((w->vector), (p->rsun), dim);
    w->cos_theta =  cos(w->v * sim->dt / p->radius);
}