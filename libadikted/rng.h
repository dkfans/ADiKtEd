/******************************************************************************/
/*
 * rng.h
 *
 * Interface functions for random number generator.
 * It can be compiled to use portable algorithm working regardless of OS/platform.
 *
 */
/******************************************************************************/

#ifndef ADIKT_RNG_H
#define ADIKT_RNG_H

#include "globals.h"


/**
 * Return maximum possible value generated by RNG.
 * For stdlib.h backend it is RAND_MAX.
 */
DLLIMPORT unsigned int rng_rand_max();

DLLIMPORT void rng_srand( unsigned int seed );

/**
 * Initialize RNG with 'unpredictable' seed. Return the seed.
 */
DLLIMPORT unsigned int rng_srand_random();

DLLIMPORT unsigned int rng_rand();


#endif /* ADIKT_RNG_H */
