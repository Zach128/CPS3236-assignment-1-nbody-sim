#ifndef NBODY_H_
#define NBODY_H_

#include "bpoint.h"

void load_nbody_params(int totalNodes, int rank, b_point *bodies, int body_count, float time_delta);
void sync_across_ranks(b_point *points);
void naive_main(b_point *bodies, int body_count, float grav_constant, float time_delta);
void move_bodies(b_point *bodies, int body_count, float time_delta);
void barnes_main(b_point *points, int bodyCount, float grav_constant, float time_delta);
#endif
