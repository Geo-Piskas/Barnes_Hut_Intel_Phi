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
} node;

void node_print(node *n);



/******************/
/*      MIC       */
/******************/
typedef struct node_mic {
    long size;
    real center[3];
    real radius;
    real mass_total;
    real mass_center[3];
    long next[8]; // Array indices.
} node_mic;

void micnode_print(node_mic *n);

#define ALLOC alloc_if(1)
#define FREE free_if(1)
#define RETAIN free_if(0)
#define REUSE alloc_if(0)

#define ALIGNED __attribute__((aligned(64)))
#endif