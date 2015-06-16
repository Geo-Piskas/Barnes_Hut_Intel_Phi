#include "octree.h"
#include <stdlib.h>
#include <stdio.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

// "3D" cube:
//  |6|7|\
//  |4|5| \
//   \ |2|3|
//    \|0|1|
inline static char find_quadrant(real* pos, real* center) {
    return 0                       // 000 = 0x0
    |  (pos[X] > center[X])        // 001 = 0x1
    | ((pos[Y] > center[Y]) << 1)  // 010 = 0x2
    | ((pos[Z] > center[Z]) << 2); // 100 = 0x4
}

// Find the center of the new quadrant:
inline static void calc_center(node* n, char index, real half_rad) {
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

// Recursively inserts a particle into the tree, creating new quadrants when necessary.
void octree_insert(node* n, particle p, long *node_count) {
    real half_rad = 0.5 * n->radius;
    
    if (n->size > 1) { // Internal node.
        // Update node's data:
        n->mass_center[:] += p.position[:] * p.mass;
        n->size += 1;
        n->mass_total += p.mass;
        
        // Insert the particle:
        char index = find_quadrant(p.position, n->center);
        if (!n->next[index]) { // Create a node with a single particle.
            n->next[index] = malloc(sizeof(node));
            n->next[index]->size = 1;
            calc_center(n, index, half_rad);
            n->next[index]->radius = half_rad;
            n->next[index]->mass_total = p.mass;
            n->next[index]->mass_center[:] = p.position[:];
            n->next[index]->next[:] = NULL;
            
            *node_count += 1;
        } else { // Propagate the insertion.
            octree_insert(n->next[index], p, node_count);
        }
    } else if (n->size == 1){ // External node (non-empty).
        // Create a node with the existing particle.
        char index = find_quadrant(n->mass_center, n->center);  // n->mass_center == p.position
        n->next[index] = malloc(sizeof(node));
        n->next[index]->size = 1;
        calc_center(n, index, half_rad);
        n->next[index]->radius = half_rad;
        n->next[index]->mass_total = n->mass_total;
        n->next[index]->mass_center[:] = n->mass_center[:];
        n->next[index]->next[:] = NULL;
        
        *node_count += 1;

        // Update node's data:
        n->mass_center[:] += p.position[:] * p.mass;
        n->size += 1;
        n->mass_total += p.mass;
        
        // Insert the particle:
        index = find_quadrant(p.position, n->center);
        if (!n->next[index]) { // Create a node with a single particle.
            n->next[index] = malloc(sizeof(node));
            n->next[index]->size = 1;
            calc_center(n, index, half_rad);
            n->next[index]->radius = half_rad;
            n->next[index]->mass_total = p.mass;
            n->next[index]->mass_center[:] = p.position[:];
            n->next[index]->next[:] = NULL;
            
            *node_count += 1;
        } else { // Propagate the insertion.
            octree_insert(n->next[index], p, node_count);
        }
    } else {
        n->size = 1;
        n->mass_total = p.mass;
        n->mass_center[:] = p.position[:];
        n->next[:] = NULL;
    }
}

long octree_transform(node_mic* microot, long *idx, node* n) {
    long i = *idx;
    *idx += 1;
    
    microot[i].size = n->size;
    microot[i].center[:] = n->center[:];
    microot[i].radius = n->radius;
    microot[i].mass_total = n->mass_total;
    
    if (microot[i].size > 1) {
        microot[i].mass_center[:] = n->mass_center[:] / n->mass_total;
        for (int j = 0; j < 8; j++) {
            if (n->next[j]) {
                microot[i].next[j] = octree_transform(microot, idx, n->next[j]);
                free(n->next[j]);
                n->next[j] = NULL;
            } else {
                microot[i].next[j] = 0;
            }
        }
    } else {
        microot[i].mass_center[:] = n->mass_center[:];
        microot[i].next[:] = 0;
    }
    return i;
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