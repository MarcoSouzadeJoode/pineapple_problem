#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "vecmath.h"
#include "quaternion_math.h"

#define PI 3.14159265358979
#define dim 3

/*
Above human wandering, the sun never sets...
-- Lao'c, Tao Te Ching

https://what-if.xkcd.com/25/

*/


struct walker {
    /*
    Stores information about the walker on the planet.
    The walker walks with a constant speed towards the Sun.
    His position is recored using a unit vector,
    coded using the vector part of the quaternion r;
    The polar representation of this position vector
    uses the radius and two spherical coordinates.
    (radius, theta, lambda).

    The velocity of the walker is recored as v in m/s,
    and is recalculated into geodesic arc-length (th_mov).

    As half of the sine and cosine of th_mov are 
    used in the rotation quaternions (versors) to shift
    the current position of the walker, they are pre-calculated
    and stored. 

    */
    struct quaternion r;
    double radius; // typically 1.00
    double theta; // latitude
    double lambda; // longitude
    double v; // speed
    double th_mov; // angle tranversed during one dt
    double S; // sin(th_mov / 2)
    double C; // cos (th_mov / 2)

};

struct planet {
    /*
    The position of the sun is stored as a vector
    (i.e., a pure quaternion) rsun. This position is
    calculated from astronomy.

    The rotation needed to move from the current walker's
    position (walker.r) to his new position is represented
    using the quaternion planet.versor .

    The spherical coordinates of the sun are represented using
    rs, theta_s and lambda_s -- coordinates of the sub-solar point.

    the struct planet also records the parameters of the planet
    and the general astronomy, i.e., the rotational and orbital
    angular velocities, the starting ecliptic longitude of the sun theta_0,
    the obliquity of the orbit etc.
    */
    struct quaternion rsun;
    struct quaternion versor;
    double radius; // radius of the planet
    double rs;
    double theta_s;
    double lambda_s; //spherical coords. of the sub-solar point
    double omega; // sidereal angular velocity of the s
    double OMG; // orbital angular velocity
    double theta_0;
    double epsilon; // obliquity
    double CEPS;
    double SEPS;
};

struct simulation_parameters {
    long N;
    double t; // current time
    double dt; //time step
    double cos_z; // cosine of zenit angle
    double alpha; // parameters used in different implementation
    double beta;
    char filename[64];
};

void sim_init(struct walker *w, struct planet *p, struct simulation_parameters *sim);
void print_to_file(struct walker *w, struct planet *p, struct simulation_parameters *sim, FILE *file);
void print_header(struct walker *w, struct planet *p, struct simulation_parameters *sim, FILE *file, FILE *metadata);


void Rsun_inc_quat(struct planet *p, struct simulation_parameters *sim){
    /*
    Calculate the cartesian coordinates of the sub-solar point
    using basic astronomy, in the classic formalism
    of Euler Angles (https://en.wikipedia.org/wiki/Euler_angles).
    */
    double CO, Co, SO, So;
    CO = cos(p->OMG*sim->t + p->theta_0);
    SO = sin(p->OMG*sim->t + p->theta_0);
    Co = cos(p->omega*sim->t);
    So = sin(p->omega*sim->t);

    p->rsun.x = +CO*Co + SO*So*p->CEPS;
    p->rsun.y = +CO*So - SO*Co*p->CEPS;
    p->rsun.z = -SO*p->SEPS;
}


void Rsun_simple_quat(struct planet *p, struct simulation_parameters *sim) {
    /*
    Calculating the solar position for a simplified (i.e., original) version
    of the problem, with zero axial inclination.
    */
    p->rsun.w = 0;
    p->rsun.x = +cos(p->omega * sim->t);
    p->rsun.y = -sin(p->omega * sim->t);
    p->rsun.z = 0;
}

void simulation_step(struct walker *w, struct planet *p, struct simulation_parameters *sim){
    struct quaternion Q = {0,0,0,0}, Qbar={0,0,0,0}, Q_temp={0,0,0,0};
    // choose simple vs complicated astronomy setting
    // just comment out the astronomy setting you dont want :D

    Rsun_inc_quat(p, sim);
    //Rsun_simple_quat(p, sim); 

    // product of two versors: (-dot product, +cross product)
    Q = q_mult(&(w->r), &(p->rsun));

    // checking sun above horizon:
    if (Q.w < 0) {
        Q.w = 0;
        q_normalize(&Q);

        // rescale versor
        Q.w  = w->C;
        Q.x *= -w->S;
        Q.y *= -w->S;
        Q.z *= -w->S;

        // perform quaternion rotation
        Qbar = q_inverse(&Q);
        Q_temp = q_mult(&(w->r), &Q);
        w->r = q_mult(&Qbar, &Q_temp);
    }
    
}

void simulation(struct walker *w, struct planet *p, struct simulation_parameters *sim) {
    /*
    Perform the solar pilgramige.
    Saving simulation metedata, i.e., astronomy & sim. parameters.
    The data is saved after each simulation step, saving to memory was impractical
    for long simulations.

    This approach has constant space complexity, overwriting data in previous steps,
    which is passed by reference through the walker, planet and sim structs.

    Polar conversion can be turned off for faster computations, and can be
    for example logged once every K iterations. 
    */
    FILE *file = fopen(sim->filename, "w");
    FILE *metadata = fopen(strcat(sim->filename, "_metadata"), "w");
    print_header(w, p, sim, file, metadata);
    fclose(metadata);

    for (int i = 0; i < sim->N; i++) {
        simulation_step(w, p, sim);
        q_to_polar(&(w->r), &(w->radius), &(w->lambda), &(w->theta));
        q_to_polar(&(p->rsun), &(p->rs), &(p->lambda_s), &(p->theta_s));
        print_to_file(w, p, sim, file);
        sim->t += sim->dt;
    }

    fclose(file);
}


int main() {
    struct planet p;
    struct walker w;
    struct simulation_parameters sim;

    sim_init(&w, &p, &sim);

    clock_t begin = clock();
    simulation(&w, &p, &sim);
    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("DONE, Time: %lf\n", time_spent);

    return 0;
}


void sim_init(struct walker *w, struct planet *p, struct simulation_parameters *sim) {
    char basefilename[] = "data/RES_";
    char fullfilename[250];
    char datetime[100];

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H-%M-%S", timeinfo);
    strcpy(fullfilename, basefilename);
    strcat(fullfilename, datetime);

    w->r.w = 0;
    w->r.x = 0.5;
    w->r.y = 0;
    w->r.z = 1.0;
    w->v = 250; // m/s

    

    p->radius = 6e6;
    p->omega = -2*PI / 86146;
    p->OMG = p->omega / 365.25;
    p->epsilon = 23.5 * PI / 180.0;
    p->theta_0 = PI;


    p->rsun.w = 0;
    p->rsun.x = 1.0;
    p->rsun.y = 0;
    p->rsun.z = 0;

    q_normalize(&(w->r));
    q_normalize(&(p->rsun));

    q_to_polar(&(w->r), &(w->radius), &(w->lambda), &(w->theta));
    q_to_polar(&(p->rsun), &(p->rs), &(p->lambda_s), &(p->theta_s));

    sim->N = 10000;
    sim->t = 0;
    sim->dt = 1000; // s


    strncpy(sim->filename, fullfilename, sizeof(sim->filename) - 1);
    sim->filename[sizeof(sim->filename) - 1] = '\0';

    w->C = cos(w->v * sim->dt / p->radius / 2.0);
    w->S = sin(w->v * sim->dt / p->radius / 2.0);

    p->CEPS = cos(p->epsilon);
    p->SEPS = sin(p->epsilon);
}

void print_header(struct walker *w, struct planet *p, struct simulation_parameters *sim, FILE *file, FILE *metadata) {

    // header for data file
    //fprintf(file, "t\ttheta\tlambda\tx\ty\tz\tthetas\tlambdas\txs\tys\tzs\n");
    fprintf(file, "t\ttheta\tlambda\tthetas\tlambdas\n");

    //create metadata file

    fprintf(metadata, "INITIAL WALKER PARAMETERS\n");
    fprintf(metadata, "THETA\t%lf\n", w->theta);
    fprintf(metadata, "LAMBDA\t%lf\n", w->lambda);
    fprintf(metadata, "VELOCITY\t%lf\n\n", w->v);

    fprintf(metadata, "ASTRONOMY PARAMETERS\n");
    fprintf(metadata, "OMEGA\t%lf\n",p->omega);
    fprintf(metadata, "OMEGA_ORB\t%lf\n",p->OMG);
    fprintf(metadata, "INITIAL_ANOMALY\t%lf\n",p->theta_0);
    fprintf(metadata, "EPSILON\t%lf\n\n",p->epsilon);

    fprintf(metadata, "SIMULATION PARAMETERS\n");
    fprintf(metadata, "SIMULATION_STEPS\t%ld\n",sim->N);
    fprintf(metadata, "TIME_STEP\t%lf\n",sim->dt);

}

void print_to_file(struct walker *w, struct planet *p,
 struct simulation_parameters *sim, FILE *file) {
    fprintf(file, "%lf\t%lf\t%lf\t%lf\t%lf\n", 
    sim->t, w->theta, w->lambda, p->theta_s, p->lambda_s);
};