#ifndef NBODY_H_
#define NBODY_H_

#include "bpoint.h"

void loadNbodyPoints(int totalNodes, int rank, b_point *bodies, int body_count, float time_delta);
void syncBodiesAcrossRanks(b_point *points);
void ComputeForces(b_point *bodies, int body_count, float grav_constant, float time_delta);
void MoveBodies(b_point *bodies, int body_count, float time_delta);
void BarnesComputeForces(b_point *bodies, int bodyCount);
#endif
