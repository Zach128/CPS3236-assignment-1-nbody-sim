#ifndef NBODY_H_
#define NBODY_H_

#include "bpoint.h"

/**
 * Function: load_nbody_params
 * ---------------------
 * Initialise the nbody simulator with the necessary parameters for simulation.
 * 
 * totalNodes: The total number of active processes.
 * rank: The rank of this process/instance.
 * body_count: How many bodies/points are going to be simulated.
 * bodies: The bodies to simulate.
 * bodyCount: How many bodies are in the array.
 * gravConstant: The gravitational constant.
 * timeDelta: How much time passes per step/iteration.
 **/
void load_nbody_params(int totalNodes, int rank, int body_count, b_point *bodies, int bodyCount, double gravConstant, double timeDelta);

/**
 * Function: sync_across_ranks
 * ---------------------
 * Synchronise the current points across all ranks of the simulator.
 **/
void sync_across_ranks();

/**
 * Function: naive_main
 * ---------------------
 * The main entrypoint for the naive implementation. This is parallelised for shared, distributed and hybrid memory environments.
 * Processes one time-step, after which will need to be synchronised across all ranks (if distributed).
 **/
void naive_main();

/**
 * Function: barnes_main
 * ---------------------
 * The main entrypoint for the Barnes-Hut implementation. This is parallelised for shared scenarios only.
 **/
void barnes_main();
#endif
