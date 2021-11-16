/*
 * rng.c
 *
 */

#include "rng.h"


#ifdef RNG_MT

    /**
     * Use Mersenne Twister implementation (portable).
     */

    #undef MT_GENERATE_CODE_IN_HEADER
    #define MT_GENERATE_CODE_IN_HEADER 0

    #include "mtwist/mtwist.h"


    unsigned int rng_rand_max() {
        return 4294967295;                      /** 2^32 - 1 **/
    }

    void rng_srand( unsigned int seed ) {
        mt_seed32( seed );
    }

    unsigned int rng_srand_random() {
        return mt_goodseed();
    }

    unsigned int rng_rand() {
        return mt_lrand();
    }

#else

    /**
     * Use standard implementation of RNG (not portable).
     */

    #include <stdlib.h>


    unsigned int rng_rand_max() {
        return RAND_MAX;
    }

    void rng_srand( unsigned int seed ) {
        srand( seed );
    }

    unsigned int rng_srand_random() {
        unsigned int seed = time(NULL);
        rng_srand( seed );
        return seed;
    }

    unsigned int rng_rand() {
        return rand();
    }

#endif /* RNG_MT */
