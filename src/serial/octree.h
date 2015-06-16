#ifndef __BarnesHut__octree__
#define __BarnesHut__octree__

#include "types.h"
#include "constants.h"

void octree_insert(node* n, particle p);

void octree_update(node* n);

void octree_free(node* n);

void octree_print(node* n);

#endif
