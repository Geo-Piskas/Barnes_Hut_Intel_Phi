#include "types.h"
#include <stdio.h>
#include "constants.h"

void particle_print(particle p) {
    printf("\nPARTICLE:\n\
           Mass: %.2lf\n\
           Position: x: %.2lf y: %.2lf z: %.2lf\n\
           Velocity: x: %.2lf y: %.2lf z: %.2lf\n\
           Force:    x: %.2lf y: %.2lf z: %.2lf\n",
           p.mass,
           p.position[X], p.position[Y], p.position[Z],
           p.velocity[X], p.velocity[Y], p.velocity[Z],
           p.force[X], p.force[Y], p.force[Z]);
}

void node_print(node *n) {
    char cc = 0;
    for(char i = 0; i < 8; i++) {
        if (n->next[i]) {
            cc += 1;
        }
    }
    printf("\nNODE:\n\
           Size: %ld\n\
           Children: %d\n\
           Mass: %.2lf\n\
           Mass Center: x: %.2lf y: %.2lf z: %.2lf\n\
           Node Center: x: %.2lf y: %.2lf z: %.2lf\n",
           n->size, cc, n->mass_total,
           n->mass_center[X], n->mass_center[Y], n->mass_center[Z],
           n->center[X], n->center[Y], n->center[Z]);
}

void micnode_print(node_mic *n) {
    char cc = 0;
    for(char i = 0; i < 8; i++) {
        if (n->next[i]) {
            cc += 1;
        }
    }
    printf("\nNODE:\n\
           Size: %ld\n\
           Children: %d\n\
           Mass: %.2lf\n\
           Mass Center: x: %.2lf y: %.2lf z: %.2lf\n\
           Node Center: x: %.2lf y: %.2lf z: %.2lf\n",
           n->size, cc, n->mass_total,
           n->mass_center[X], n->mass_center[Y], n->mass_center[Z],
           n->center[X], n->center[Y], n->center[Z]);
}