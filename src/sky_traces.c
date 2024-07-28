#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>
#define PI 3.14159265358979

#define MAX_LINE 300


struct state {
    double t, theta, lambda, theta_s, lambda_s;
};

struct sky_state {
    /*
    time, zenith distance, azimuth
    */
    double t, z, A;
};

void zenith_distance(struct state *st, struct sky_state *sky) {
    double cosz, delta_lambda;

    delta_lambda = st->lambda - st->lambda_s;
    cosz = sin(st->theta)*sin(st->theta_s) + cos(st->theta)*cos(st->theta_s)*cos(delta_lambda);
    //printf("%f\n",cosz);
    sky->z = acos(cosz);
}

void azimuth(struct state *st, struct sky_state *sky) {
    double delta_lambda, sinA, cosA;

    delta_lambda = st->lambda - st->lambda_s;
    cosA = (sin(st->theta_s) - sin(st->theta)*cos(sky->z)) / (cos(st->theta) * sin(sky->z));
    sinA = cos(st->theta_s) * sin(delta_lambda) / sin(sky->z);
    sky->A = atan2(sinA, cosA);
}

int parse_line(struct state *st, FILE *fptr){
    int end;
    char line[MAX_LINE];
    const char tab[2] = "\t";
    char * tmp_token;

    end = 1;
    if (fgets(line, MAX_LINE, fptr) == NULL) {
        end = 0;
    }

    tmp_token = strtok(line, tab);
    if (tmp_token != NULL) {
    st->t = strtod(tmp_token, NULL);
    }


    tmp_token = strtok(NULL, tab);
    if (tmp_token != NULL) {
    st->theta = strtod(tmp_token, NULL);
    }

    tmp_token = strtok(NULL, tab);
    if (tmp_token != NULL) {
    st->lambda = strtod(tmp_token, NULL);
    }

    tmp_token = strtok(NULL, tab);
    if (tmp_token != NULL) {
    st->theta_s = strtod(tmp_token, NULL);
    }

    tmp_token = strtok(NULL, tab);
    if (tmp_token != NULL) {
    st->lambda_s = strtod(tmp_token, NULL);
    }

    //printf("%f\t%f\t%f\t%f\t%f\n", st->t, st->theta, st->lambda, st->theta_s, st->lambda_s);

    return end;
}

void write_line(struct sky_state *sky, struct state *st, FILE *fptr){
    fprintf(fptr,"%f\t%f\t%f\t%f\t%f\t%f\t%f\n", sky->t, sky->z, sky->A, st->theta, st->lambda, st->theta_s, st->lambda_s);
}



void calculate(struct state *st, struct sky_state *sky) {
    char line[MAX_LINE];
    FILE *data_file_ptr = fopen("../data/RES_2024-07-28_19-37-42", "r");
    FILE *result_file_ptr = fopen("../sky_data/2024-07-28_19-37-42.txt", "w");

    fprintf(result_file_ptr, "t\tz\tA\ttheta\tlambda\ttheta_s\tlambda_s\n");

    if (data_file_ptr != NULL) {
    fgets(line, sizeof(line), data_file_ptr);
    printf("HEADER : %s", line);

    while(parse_line(st, data_file_ptr)) {

        sky->t = st->t;
        zenith_distance(st, sky);
        azimuth(st, sky);
        //printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\n", sky->t, sky->z, sky->A, st->theta, st->lambda, st->theta_s, st->lambda_s);

        write_line(sky, st, result_file_ptr);
    }

    printf("finished\n");

    } else {
        printf("filename not recognized");
    }

    fclose(data_file_ptr);
    fclose(result_file_ptr);

}

int main() {
    struct sky_state sky;
    struct state st;

    calculate(&st, &sky);
    return 0;
}