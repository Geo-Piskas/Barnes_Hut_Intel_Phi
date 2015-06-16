#ifndef __BarnesHut__physics__
#define __BarnesHut__physics__

#include "types.h"
#include "constants.h"

void physics_calc_force(node* n, particle* p);
void physics_apply_force(particle* p, real* max_lim, real* min_lim);

#endif
