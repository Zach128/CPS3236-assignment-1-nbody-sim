#ifndef NBODY_H_
#define NBODY_H_

#include "bpoint.h"

#define FIELD_WIDTH = 1000;
#define FIELD_HALF_WIDTH = FIELD_WIDTH >> 1;
#define FIELD_HEIGHT = 1000;
#define FIELD_HALF_HEIGHT = FIELD_HEIGHT >> 1;

void ComputeForces(const b_point *bodies, int body_count, float grav_constant, float time_delta);

#endif
