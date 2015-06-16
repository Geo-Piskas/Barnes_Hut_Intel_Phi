#include "physics.h"
#include <math.h>

// Formula: a_i = G * sum[(m_j * r_ij) / (r_ij^2 + EPS^2)^(3/2)]
// G is multiplied at the update step.
void physics_calc_force(node* n, particle* p) {
    real dx = n->mass_center[X] - p->position[X];
    real dy = n->mass_center[Y] - p->position[Y];
    real dz = n->mass_center[Z] - p->position[Z];
    real dist = dx * dx + dy * dy + dz * dz;
    if (dist < EPS2) {
        dist += EPS2;
    }
    
    if (n->size == 1 || ((n->radius / sqrt(dist)) < THETA)) { // External node or close Interlan node
        real force = n->mass_total / sqrt(dist * dist * dist);
        p->force[X] += force * dx;
        p->force[Y] += force * dy;
        p->force[Z] += force * dz;
    } else { // Otherwise visit all internal node children.
        for (char i = 0; i < 8; i++) {
            if (n->next[i]) {
                physics_calc_force(n->next[i], p);
            }
        }
    }
}

void physics_apply_force(particle* p, real* max_lim, real* min_lim) {
    
    // Update the position.
    p->force[X] *= G;
    p->force[Y] *= G;
    p->force[Z] *= G;
    
    p->position[X] += (p->velocity[X] + 0.5 * p->force[X] * TIME_STEP) * TIME_STEP;
    p->position[Y] += (p->velocity[Y] + 0.5 * p->force[Y] * TIME_STEP) * TIME_STEP;
    p->position[Z] += (p->velocity[Z] + 0.5 * p->force[Z] * TIME_STEP) * TIME_STEP;
    
    // Update the velocity.
    p->velocity[X] += p->force[X] * TIME_STEP;
    p->velocity[Y] += p->force[Y] * TIME_STEP;
    p->velocity[Z] += p->force[Z] * TIME_STEP;
    
    // Reset the force.
    p->force[X] = 0.0;
    p->force[Y] = 0.0;
    p->force[Z] = 0.0;
    
    // Bounce.
    for (char i = 0; i < 3 ;i++) {
        if (p->position[i] >= max_lim[i]) {
            p->position[i] = max_lim[i] - (p->position[i] - max_lim[i]) * 0.0001;
            p->velocity[i] *= -BOUNCE_FACTOR;
        } else if (p->position[i] <= min_lim[i]) {
            p->position[i] = min_lim[i] - (p->position[i] - min_lim[i]) * 0.0001;
            p->velocity[i] *= -BOUNCE_FACTOR;
        }
    }
}