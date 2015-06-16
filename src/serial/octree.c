#include "octree.h"
#include <stdlib.h>
#include <stdio.h>


// "3D" cube:
//  |6|7|\
//  |4|5| \
//   \ |2|3|
//    \|0|1|
inline static char find_quadrant(real* pos, real* center) {
    return 0                     // 000 = 0x0
    |  (pos[X] > center[X])        // 001 = 0x1
    | ((pos[Y] > center[Y]) << 1)  // 010 = 0x2
    | ((pos[Z] > center[Z]) << 2); // 100 = 0x4
}

// Find the center of the new quadrant:
inline static void calc_center(node* n, char index) {
    real half_rad = n->radius / 2.0;
    
    if (index & 0x1) {
        n->next[index]->center[X] = n->center[X] + half_rad;
    } else {
        n->next[index]->center[X] = n->center[X] - half_rad;
    }
    
    if (index & 0x2) {
        n->next[index]->center[Y] = n->center[Y] + half_rad;
    } else {
        n->next[index]->center[Y] = n->center[Y] - half_rad;
    }
    
    if (index & 0x4) {
        n->next[index]->center[Z] = n->center[Z] + half_rad;
    } else {
        n->next[index]->center[Z] = n->center[Z] - half_rad;
    }
}

inline static void particle_insert(node* n, particle p) {
    // Update node's data:
    n->mass_center[X] += (p.position[X] * p.mass);
    n->mass_center[Y] += (p.position[Y] * p.mass);
    n->mass_center[Z] += (p.position[Z] * p.mass);
    n->size += 1;
    n->mass_total += p.mass;
    
    // Insert the particle:
    char index = find_quadrant(p.position, n->center);
    if (!n->next[index]) { // Create a node with a single particle.
        n->next[index] = malloc(sizeof(node));
        n->next[index]->size = 1;
        calc_center(n, index);
        n->next[index]->radius = n->radius / 2.0;
        n->next[index]->mass_total = p.mass;
        n->next[index]->mass_center[X] = p.position[X];
        n->next[index]->mass_center[Y] = p.position[Y];
        n->next[index]->mass_center[Z] = p.position[Z];
        NULL_NEXT(n->next[index]->next);
    } else { // Propagate the insertion.
        octree_insert(n->next[index], p);
    }
}

// Recursively inserts a particle into the tree, creating new quadrants when necessary.
void octree_insert(node* n, particle p) {
    if (n->size > 1) { // Internal node.
        particle_insert(n, p);
    } else { // External node (non-empty).
        // Create a node with the existing particle.
        char index = find_quadrant(n->mass_center, n->center);  // n->mass_center == p.position
        n->next[index] = malloc(sizeof(node));
        n->next[index]->size = 1;
        calc_center(n, index);
        n->next[index]->radius = n->radius / 2.0;
        n->next[index]->mass_total = n->mass_total;
        n->next[index]->mass_center[X] = n->mass_center[X];
        n->next[index]->mass_center[Y] = n->mass_center[Y];
        n->next[index]->mass_center[Z] = n->mass_center[Z];
        NULL_NEXT(n->next[index]->next);
        
        particle_insert(n, p);
    }
}

void octree_update(node* n) {
    // Update the center of mass:
    n->mass_center[X] /= n->mass_total;
    n->mass_center[Y] /= n->mass_total;
    n->mass_center[Z] /= n->mass_total;
    for (char i = 0; i < 8; i++) {
        if (n->next[i] && n->next[i]->size > 1) {
            octree_update(n->next[i]);
        }
    }
}

void octree_free(node* n) {
    for (char i = 0; i < 8; i++) {
        if (n->next[i]) {
            octree_free(n->next[i]);
            n->next[i] = NULL;
        }
    }
    free(n);
    n = NULL;
}

void octree_print(node* n) {
    if (n) {
        node_print(n);
        for (char i = 0; i < 8; i++) {
            if (n->next[i]) {
                printf("Child %d: ", i);
                octree_print(n->next[i]);
            }
        }
    }
}