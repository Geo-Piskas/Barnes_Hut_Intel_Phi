#define _GNU_SOURCE
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

ALIGNED static long particle_count;
ALIGNED static particle* ps;
ALIGNED static char dir_name[100];

ALIGNED static real min[3] = {HUGE_VALL, HUGE_VALL, HUGE_VALL};
ALIGNED static real max[3] = {-HUGE_VALL, -HUGE_VALL, -HUGE_VALL};
ALIGNED static real center[3];
ALIGNED static real radius;

static inline void write_plot_limits() {
    char filename[100];
    sprintf(filename, "%s/plot.cfg", dir_name);
    
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Could not write file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%lf %lf %lf %lf %lf %lf", center[X] - radius, center[X] + radius,
                                           center[Y] - radius, center[Y] + radius,
                                           center[Z] - radius, center[Z] + radius);
    fclose(fp);
}

static inline void rand_string(char *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}

static inline void update_minmax(real v[3]) {
    // Max
    if (v[:] > max[:]) max[:] = v[:];
    // Min
    if (v[:] < min[:]) min[:] = v[:];
}

static inline void calculate_cube() {
    radius = 0.5 * RADIUS_FACTOR * __sec_reduce_max(max[:] - min[:]);
    center[:] = 0.5 * (max[:] + min[:]);
}

particle* read_particles(const char* file) {
    char postfix[10];
    srand((unsigned)time(NULL));
    rand_string(postfix, 10);
    sprintf(dir_name, "out_%s_%s", basename((char*)file), postfix);
    mkdir(dir_name, 0777);
    
    ps = NULL;
    char* line = NULL;
    FILE* fp = fopen(file, "r");
    size_t len = 0;
    
    if (!fp) {
        printf("Invalid input: %s\nCould not open file.\n", file);
        exit(EXIT_FAILURE);
    }
    
    if (getline(&line, &len, fp) != -1) {
        char* end;
        particle_count = strtol(line, &end, 10);
        if (*end == 0) {
            printf("Invalid input: %s\nExpected number of particles.\n", line);
            exit(EXIT_FAILURE);
        }
        
        ps = malloc(particle_count * sizeof(particle));
    } else {
        printf("Invalid input: Empty file.\n");
        exit(EXIT_FAILURE);
    }
    
    // m x y z vx vy vz
    for (long i = 0; i < particle_count; i++) {
        getline(&line, &len, fp);
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf", &ps[i].mass,
                                                    &ps[i].position[X], &ps[i].position[Y], &ps[i].position[Z],
                                                    &ps[i].velocity[X], &ps[i].velocity[Y], &ps[i].velocity[Z]);
        if (ps[i].mass == 0) {
            printf("Invalid input: Invald particle at line %ld.\n", i + 1);
            exit(EXIT_FAILURE);
        }
        
        ps[i].force[:] = 0.0;
        
        update_minmax(ps[i].position);
    }
    
    calculate_cube();
    
    fclose(fp);
    if (line) {
        free(line);
    }
    
    write_plot_limits();
    write_particles(-1); // Initial state of particles.
    return ps;
}

void write_particles(real time) {
    char filename[100];
    sprintf(filename, "%s/%020.5lf.out", dir_name, time);
    
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Could not write file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    for (long i = 0; i < particle_count; i++) {
        fprintf(fp, "%lf %lf %lf\n", ps[i].position[X], ps[i].position[Y], ps[i].position[Z]);
    }
    
    fclose(fp);
}

real get_sim_time(const char* arg) {
    char* end;
    real time = strtod(arg, &end);
    if (*end) {
        printf("Invalid input: %s\nExpected number.\n", arg);
        exit(EXIT_FAILURE);
    }
    return time * YEAR; // res * a year
}

long get_particle_count() {
    return particle_count;
}

void get_center(real c[3]) {
    c[:] = center[:];
}

real get_radius() {
    return radius;
}

