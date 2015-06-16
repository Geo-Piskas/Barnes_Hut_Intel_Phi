#ifndef __BarnesHut__physics__
#define __BarnesHut__physics__

#include "types.h"
#include "constants.h"

__attribute__((target(mic))) void physics_calc_force_mic(node_mic* root, node_mic* n, particle* p);

__attribute__((target(mic))) void physics_apply_force(particle* p, real* max_lim, real* min_lim);

#endif
