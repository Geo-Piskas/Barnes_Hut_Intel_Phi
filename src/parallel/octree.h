#ifndef __BarnesHut__octree__
#define __BarnesHut__octree__

#include "types.h"
#include "constants.h"

void octree_insert(node* n, particle p, long *node_count);

void octree_update(node* n);

long octree_transform(node_mic* microot, long *idx, node* root);

void octree_free(node* n);

void octree_print(node* n);

#endif
