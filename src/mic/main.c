#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "io.h"
#include "types.h"
#include "constants.h"
#include "octree.h"
#include "physics.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>


// argv[1]: filename
// argv[2]: time in years
int main(int argc, const char* argv[]) {
    if (argc < 3) {
        printf("Missing argument: dataset and time are required.\n");
        exit(EXIT_FAILURE);
    }
    ALIGNED double begin = omp_get_wtime();
    
    ALIGNED particle* particles = read_particles(argv[1]);
    ALIGNED const real TIME_LIMIT = get_sim_time(argv[2]);
    ALIGNED long N = get_particle_count();

    ALIGNED real center[3];
    get_center(center);
    ALIGNED real radius = get_radius();
    ALIGNED real max_lim[3];
    ALIGNED real min_lim[3];
    max_lim[:] = center[:] + radius;
    min_lim[:] = center[:] - radius;
    
    ALIGNED node* root[2];
    ALIGNED int curr = 0;
    ALIGNED int output_count = 0;
    for (real time = 0.0; time < TIME_LIMIT; time += TIME_STEP) {
        
#ifdef TIME
        begin = omp_get_wtime();
#endif
        
        // Root of the octree, new per iteration.
        root[curr] = _mm_malloc(sizeof(node), 64);
        root[curr]->center[:] = center[:];
        root[curr]->radius = radius;
        root[curr]->size = 1;
        root[curr]->mass_total = particles[0].mass;
        root[curr]->mass_center[:] = particles[0].position[:];
        root[curr]->next[:] = NULL;
        
        // Insert each particle into the tree. In the meantime, the physical properties of
        // the particle are shared with nodes in the path from the root until the node’s position.
        for (long i = 1; i < N; i++) { // Particle 0 already inserted at root.
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
        #pragma omp parallel for schedule(static)
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
    printf("Native: %f\n", omp_get_wtime() - begin);
#endif
    
    // Free particles array.
    _mm_free(particles);
    return 0;
}