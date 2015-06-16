#ifndef __BarnesHut__types__
#define __BarnesHut__types__


/******************/
/*      REAL      */
/******************/
typedef double real;



/******************/
/*    PARTICLE    */
/******************/
typedef struct particle {
    real mass; // kg
    real position[3]; // m
    real velocity[3]; // m/s
    real force[3]; // m/s^2
    real padding[6]; // 64-bit aligned (128)
} particle;


void particle_print(particle n);



/******************/
/*      NODE      */
/******************/
#define NULL_NEXT(next) { next[0] = next[1] = next[2] = next[3] = next[4] = next[5] = next[6] = next[7] = NULL;}

typedef struct node {
    long size;
    real center[3];
    real radius;
    real mass_total;
    real mass_center[3];
    struct node* next[8];
    real padding[7]; // 64-bit aligned (192)
} node;

void node_print(node *n);

#define ALIGNED __attribute__((aligned(64)))
#endif
