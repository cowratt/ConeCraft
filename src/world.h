#ifndef _world_h_
#define _world_h_

typedef void (*world_func)(int, int, int, int, void *);

void create_world(int p, int q, world_func func, void *arg);
void gen_tree(int x, int z, int ground_height, world_func func, void *arg);
void gen_cone(int x, int z, int ground_height, world_func func, void *arg);
#endif
