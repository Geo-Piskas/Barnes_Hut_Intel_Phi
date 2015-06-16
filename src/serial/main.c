#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "io.h"
#include "types.h"
#include "constants.h"
#include "octree.h"
#include "physics.h"


// argv[1]: filename
// argv[2]: time in years
int main(int argc, const char* argv[]) {
    if (argc < 3) {
        printf("Missing argument: dataset and time are required.\n");
        exit(EXIT_FAILURE);
    }
    double begin = omp_get_wtime();

    particle* particles = read_particles(argv[1]);
    const real TIME_LIMIT = get_sim_time(argv[2]);
    long N = get_particle_count();
    
    real center[3];
    get_center(center);
    real radius = get_radius();
    real max_lim[3] = {center[X] + radius, center[Y] + radius, center[Z] + radius};
    real min_lim[3] = {center[X] - radius, center[Y] - radius, center[Z] - radius};
    
    node* root[2];
    char curr = 0;
    int output_count = 0;
    for (real time = 0.0; time < TIME_LIMIT; time += TIME_STEP) {
        
#ifdef TIME
        begin = omp_get_wtime();
#endif

        // Root of the octree, new per iteration.
        root[curr] = malloc(sizeof(node));
        root[curr]->center[X] = center[X];
        root[curr]->center[Y] = center[Y];
        root[curr]->center[Z] = center[Z];
        root[curr]->radius = radius;
        root[curr]->size = 1;
        root[curr]->mass_total = particles[0].mass;
        root[curr]->mass_center[X] = particles[0].position[X];
        root[curr]->mass_center[Y] = particles[0].position[Y];
        root[curr]->mass_center[Z] = particles[0].position[Z];
        NULL_NEXT(root[curr]->next);
        
        // Insert each particle into the tree. In the meantime, the physical properties of
        // the particle are shared with nodes in the path from the root until the node’s position.
        for (long i = 1; i < N; i++) { // Particle 0 already inserted.
            octree_insert(root[curr], particles[i]);
        }
        
        // Calculate the final center of mass of each node.
        octree_update(root[curr]);
        
#ifdef TIME
        printf("INS: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        // Calculate the force on each particle.
        // Update each particle's physical properties based on the given environment constants.
        for (long i = 0; i < N; i++) {
            physics_calc_force(root[curr], &particles[i]);
            physics_apply_force(&particles[i], max_lim, min_lim);
        }
        
#ifdef TIME
        printf("PHY: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        // Write the new particles.
        if (time >= (output_count * OUTPUT_STEP)) {
            write_particles(time);
            output_count += 1;
        }
        
#ifdef TIME
        printf("WRT: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        // Move to the next tree bucket and free the current.
        octree_free(root[curr]);
        curr = (curr + 1) % 2;
        
#ifdef TIME
        printf("FRE: %f\n\n", omp_get_wtime() - begin);
#endif
    }
#ifndef TIME
    printf("Serial: %f\n", omp_get_wtime() - begin);
#endif
    
    // Free particles array.
    free(particles);
    return 0;
}