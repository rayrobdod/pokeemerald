#include "constants/moves.h"

const struct TechniqueManualPage gTechniqueManualPages[TM_COUNT] =
{
    [TM_TACKLE] =
    {
        .move = MOVE_TACKLE,
        .tasks = {
            {
                .type = TM_TASK_SEEN_DIFFERENT_SPECIES,
                .storage_index = TM_SPECIES_TACKLE,
                .requirement = 3,
            },
            {
                .type = TM_TASK_SEEN,
                .storage_index = TM_COUNTER_TACKLE_SEEN,
                .requirement = 20,
            },
        }
    },
    [TM_POUND] =
    {
        .move = MOVE_POUND,
        .tasks = {
            {
                .type = TM_TASK_SEEN_DIFFERENT_SPECIES,
                .storage_index = TM_SPECIES_POUND,
                .requirement = 3,
            },
            {
                .type = TM_TASK_SEEN,
                .storage_index = TM_COUNTER_POUND_SEEN,
                .requirement = 20,
            },
        }
    },
    [TM_THUNDER] =
    {
        .move = MOVE_THUNDER,
        .tasks = {
            {
                .type = TM_TASK_SEEN_DIFFERENT_SPECIES,
                .storage_index = TM_SPECIES_THUNDER,
                .requirement = 3,
            },
            {
                .type = TM_TASK_SEEN,
                .storage_index = TM_COUNTER_THUNDER_SEEN,
                .requirement = 30,
            },
            {
                .type = TM_TASK_SEEN_RAIN,
                .storage_index = TM_COUNTER_THUNDER_SEEN_RAIN,
                .requirement = 3,
            },
        }
    },
    [TM_SOLAR_BEAM] =
    {
        .move = MOVE_SOLAR_BEAM,
        .tasks = {
            {
                .type = TM_TASK_SEEN_DIFFERENT_SPECIES,
                .storage_index = TM_SPECIES_SOLAR_BEAM,
                .requirement = 3,
            },
            {
                .type = TM_TASK_SEEN,
                .storage_index = TM_COUNTER_SOLAR_BEAM_SEEN,
                .requirement = 30,
            },
            {
                .type = TM_TASK_SEEN_SUN,
                .storage_index = TM_COUNTER_SOLAR_BEAM_SEEN_SUN,
                .requirement = 3,
            },
        }
    },
    [TM_CHILLING_WATER] =
    {
        .move = MOVE_CHILLING_WATER,
        .tasks = {
            {
                .type = TM_TASK_SPECIAL_COUNTER,
                .storage_index = TM_COUNTER_WATER_USING_ICE,
                .requirement = 30,
                .description = COMPOUND_STRING("See a Water Pokémon\nuse an Ice move"),
            },
        }
    },
    [TM_CUT] =
    {
        .move = MOVE_CUT,
        .tasks = {
            [0] = {
                .type = TM_TASK_SCRIPT_FLAG,
                .storage_index = TM_FLAG_CUTTER,
                .requirement = 1,
                .description = COMPOUND_STRING("Learn from The Cutter"),
            },
        }
    },
    [TM_ROCK_TOMB] =
    {
        .move = MOVE_ROCK_TOMB,
        .tasks = {
            {
                .type = TM_TASK_SCRIPT_FLAG,
                .storage_index = TM_FLAG_ROXANNE,
                .requirement = 1,
                .description = COMPOUND_STRING("Learn from Leader\nRoxanne"),
            },
        }
    },
};
