#ifndef __BarnesHut__io__
#define __BarnesHut__io__

#include "types.h"
#include "constants.h"

real get_sim_time(const char* arg);

particle* read_particles(const char* file);
void write_particles(real time);

long get_particle_count();
void get_center(real* c);
real get_radius();

#endif
