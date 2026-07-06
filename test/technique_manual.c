#include "global.h"
#include "test/test.h"
#include "move.h"
#include "pokemon.h"
#include "technique_manual.h"

TEST("Technique Manual: Each teachable move is in someone's learnset")
{
    enum Move move = MOVE_UNAVAILABLE;

    for (enum TmPages i = 0; i < TM_COUNT; i++)
    {
        enum Move move_i = gTechniqueManualPages[i].move;
        PARAMETRIZE_LABEL("%S", gMovesInfo[move_i].name) { move = move_i; }
    }

    bool32 not_found = TRUE;
    for (u32 i = 0; i < NUM_SPECIES && not_found; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            for (
                const u16 *teachableLearnset = gSpeciesInfo[i].teachableLearnset;
                MOVE_UNAVAILABLE != *teachableLearnset;
                teachableLearnset++)
            {
                if (move == *teachableLearnset)
                {
                    not_found = FALSE;
                    break;
                }
            }
        }
    }
    EXPECT(!not_found);
}

TEST("Technique Manual: All used flags/counters/species are in-bounds")
{
    for (enum TmPages page = 0; page < TM_COUNT; page++)
    for (unsigned task_id = 0; task_id < TASKS_PER_PAGE; task_id++)
    {
        const struct ResearchTask *task = &gTechniqueManualPages[page].tasks[task_id];
        switch (task->type) {
        case TM_TASK_NONE:
        case TM_TASK_COUNT:
            break;
        case TM_TASK_SCRIPT_FLAG:
            EXPECT(task->storage_index < TM_FLAGS_COUNT);
            break;
        case TM_TASK_MASTER_MOVE:
            EXPECT(task->storage_index < TM_TASK_COUNT);
            break;
        case TM_TASK_SEEN_DIFFERENT_SPECIES:
            EXPECT(task->storage_index + task->requirement < TM_SPECIES_COUNT);
            EXPECT(task->requirement < MAX_SPECIES_REQUIREMENT);
            break;
        case TM_TASK_SPECIAL_COUNTER:
        case TM_TASK_SEEN:
        case TM_TASK_SEEN_RAIN:
        case TM_TASK_SEEN_SUN:
            EXPECT(task->storage_index < TM_COUNTERS_COUNT);
            break;
        }
    }

    if (0 <= TM_COUNTER_WATER_USING_ICE) {
        EXPECT(TM_COUNTER_WATER_USING_ICE < TM_COUNTERS_COUNT);
    }
}
