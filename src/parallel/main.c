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
    double begin = omp_get_wtime();
    particle* particles = read_particles(argv[1]);
    ALIGNED const real TIME_LIMIT = get_sim_time(argv[2]);
    ALIGNED long N = get_particle_count();
    ALIGNED long output_count = 0;

    ALIGNED real center[3];
    get_center(center);
    ALIGNED real radius = get_radius();
    ALIGNED real half_rad = 0.5 * radius;
    ALIGNED real max_lim[3];
    ALIGNED real min_lim[3];
    max_lim[:] = center[:] + radius;
    min_lim[:] = center[:] - radius;
    
#ifdef TIME
    begin = omp_get_wtime();
#endif
    
    // Transfer and persist data on the mic.
    #pragma offload_transfer target(mic) in(particles : length(N) ALLOC RETAIN) \
                                         in(max_lim   :           ALLOC RETAIN) \
                                         in(min_lim   :           ALLOC RETAIN)

#ifdef TIME
    printf("SND: %f\n\n", omp_get_wtime() - begin);
#endif

    // Node count per child of the root.
    ALIGNED long node_count[8];
    ALIGNED long total_nodes;

    // The first 8 children of the octree.
    ALIGNED node roots[8];
    
    // Children creation and initialization.
    for (char i = 0; i < 8; i++) {
        roots[i].radius = half_rad;
        if (i & 0x1) {
            roots[i].center[X] = center[X] + half_rad;
        } else {
            roots[i].center[X] = center[X] - half_rad;
        }
        
        if (i & 0x2) {
            roots[i].center[Y] = center[Y] + half_rad;
        } else {
            roots[i].center[Y] = center[Y] - half_rad;
        }
        
        if (i & 0x4) {
            roots[i].center[Z] = center[Z] + half_rad;
        } else {
            roots[i].center[Z] = center[Z] - half_rad;
        }
    }

    // Copyable version of the octree. Needed for MIC offloading.
    ALIGNED node_mic* microot;
    ALIGNED long micmemptrs[8];
    
    // Locks used at parallel tree creation.
    omp_lock_t lock[8];
    omp_init_lock(&lock[:]);
    
    for (real time = 0.0; time < TIME_LIMIT; time += TIME_STEP) {
        
#ifdef TIME
        begin = omp_get_wtime();
#endif
        
        // Resetting for the next iteration.
        node_count[:] = 1;
        roots[:].size = 0;
        total_nodes = 1;
        
        // Insert each particle into the tree in parallel. In the meantime, the physical properties of
        // the particle are shared with nodes in the path from the root until the node’s position.
        #pragma omp parallel for schedule(static)
        for (long i = 0; i < N; i++) {
            char idx = 0                                                 // 000 = 0x0
                        |  (particles[i].position[X] > center[X])        // 001 = 0x1
                        | ((particles[i].position[Y] > center[Y]) << 1)  // 010 = 0x2
                        | ((particles[i].position[Z] > center[Z]) << 2); // 100 = 0x4
            omp_set_lock(&lock[idx]);
            octree_insert(&roots[idx], particles[i], &node_count[idx]);
            omp_unset_lock(&lock[idx]);
        }
        
        // Update total node count:
        for (char i = 0; i < 8; i++) {
            if (roots[i].size != 0) {
                total_nodes += node_count[i];
            }
        }
        
#ifdef TIME
        printf("INS: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        // Convert the octree to an array.
        microot = malloc(total_nodes * sizeof(node_mic));
        microot->size = 0;
        microot->mass_total = 0;
        microot->mass_center[:] = 0;
        
        // Initialize microot.
        microot->center[:] = center[:];
        microot->radius = radius;
        for (char i = 0; i < 8; i++) {
            if (roots[i].size) {
                microot->size += roots[i].size;
                microot->mass_total += roots[i].mass_total;
                microot->mass_center[:] += roots[i].mass_center[:];
            }
        }
        microot->mass_center[:] /= microot->mass_total;
        
        // Set the chunks of memory that children start from.
        microot->next[0] = 1;
        micmemptrs[0] = 1;
        for (long i = 1; i < 8; i++) {
            microot->next[i] = microot->next[i-1] + node_count[i-1];
            micmemptrs[i] = microot->next[i];
        }
        
        // Tree to array in parallel.
        #pragma omp parallel for schedule(static)
        for (long i = 0; i < 8; i++) {
            octree_transform(microot, &micmemptrs[i], &roots[i]);
        }

#ifdef TIME
        printf("TRA: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        // Calculate the force on each particle.
        // Update each particle's physical properties based on the given environment constants.
        #pragma offload target(mic) in(microot     : length(total_nodes) ALLOC FREE) \
                                    out(particles  : length(N)           REUSE RETAIN) \
                                    nocopy(max_lim :                     REUSE RETAIN) \
                                    nocopy(min_lim :                     REUSE RETAIN)
        {
            #pragma omp parallel for schedule(static)
            for (long i = 0; i < N; i++) {
                physics_calc_force_mic(microot, microot, &particles[i]);
                physics_apply_force(&particles[i], max_lim, min_lim);
            }
        }
        
#ifdef TIME
        printf("PHY: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        // Write the new particels.
        if (time >= (output_count * OUTPUT_STEP)) {
            write_particles(time);
            output_count += 1;
        }
        
#ifdef TIME
        printf("WRT: %f\n", omp_get_wtime() - begin);
        begin = omp_get_wtime();
#endif
        
        free(microot);

#ifdef TIME
        printf("FRE: %f\n\n", omp_get_wtime() - begin);
#endif
    }
#ifndef TIME
    printf("Parall: %f\n", omp_get_wtime() - begin);
#endif

    // Free particles array.
    free(particles);
    omp_destroy_lock(&lock[:]);

    //Free particles, min and max lim form the MIC.
    #pragma offload_transfer target(mic) nocopy(particles : length(N) REUSE FREE) \
                                         nocopy(max_lim   :           REUSE FREE) \
                                         nocopy(min_lim   :           REUSE FREE)
    return 0;
}