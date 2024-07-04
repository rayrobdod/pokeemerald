#include "constants/moves.h"

const struct TechniqueManual sTM[TM_COUNT] =
{
    [TM_TACKLE] =
    {
        .move = MOVE_TACKLE,
        .species = {TM_SPECIESCOUNTER_SEEN, 3},
        .counter = {
            [0] = {TM_COUNTER_SEEN, 20},
            [1 ... TM_COUNTERS_COUNT-1] = {TM_COUNTER_NONE, 0},
        },
    },
    [TM_POUND] =
    {
        .move = MOVE_POUND,
        .species = {TM_SPECIESCOUNTER_SEEN, 3},
        .counter = {
            [0] = {TM_COUNTER_SEEN, 20},
            [1 ... TM_COUNTERS_COUNT-1] = {TM_COUNTER_NONE, 0},
        },
    },
    [TM_THUNDER] =
    {
        .move = MOVE_THUNDER,
        .species = {TM_SPECIESCOUNTER_SEEN, 3},
        .counter = {
            [0] = {TM_COUNTER_SEEN, 30},
            [1] = {TM_COUNTER_SEEN_RAIN, 3},
            [2 ... TM_COUNTERS_COUNT-1] = {TM_COUNTER_NONE, 0},
        },
    },
    [TM_SOLAR_BEAM] =
    {
        .move = MOVE_SOLAR_BEAM,
        .species = {TM_SPECIESCOUNTER_SEEN, 3},
        .counter = {
            [0] = {TM_COUNTER_SEEN, 30},
            [1] = {TM_COUNTER_SEEN_SUN, 3},
            [2 ... TM_COUNTERS_COUNT-1] = {TM_COUNTER_NONE, 0},
        },
    },
    [TM_CHILLING_WATER] =
    {
        .move = MOVE_CHILLING_WATER,
        .species = {TM_SPECIESCOUNTER_NONE, 5},
        .counter = {
            [0] = {TM_COUNTER_WATER_USING_ICE, 30},
            [1 ... TM_COUNTERS_COUNT-1] = {TM_COUNTER_NONE, 0},
        },
    },
    [TM_CUT] =
    {
        .move = MOVE_CUT,
        .species = {TM_SPECIESCOUNTER_NONE, 0},
        .counter = {
            [0] = {TM_COUNTER_BE_TUTORED, 1},
            [1 ... TM_COUNTERS_COUNT-1] = {TM_COUNTER_NONE, 0},
        },
    },
};
