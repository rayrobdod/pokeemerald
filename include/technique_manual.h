#ifndef GUARD_TECHNIQUE_MANUAL_H
#define GUARD_TECHNIQUE_MANUAL_H

#include "global.h"
#include "constants/technique_manual.h"
#include "constants/technique_manual_flags.h"
#include "constants/technique_manual_pages.h"

enum ResearchTaskType
{
    TM_TASK_NONE,
    TM_TASK_SCRIPT_FLAG,
    TM_TASK_SPECIAL_COUNTER,
    TM_TASK_SEEN,
    TM_TASK_SEEN_DIFFERENT_SPECIES,
    TM_TASK_SEEN_RAIN,
    TM_TASK_SEEN_SUN,

    TM_TASK_COUNT,
};

struct ResearchTask
{
    enum ResearchTaskType type;
    u8 storage_index;
    u8 requirement;
    const u8* description;
};

struct TechniqueManualPage
{
    enum Move move;
    struct ResearchTask tasks[TASKS_PER_PAGE];
};

extern const struct TechniqueManualPage gTechniqueManualPages[TM_COUNT];

/** Increments counters about the currently-being-used battle move */
void TmIncrementSeenStats(enum Move move, enum Species attackerSpecies);
/** Sets a TM flag */
void TmSetFlag(enum TmFlags tmFlagIndex);
/** Returns whether a TM flag has been set */
bool8 TmIsFlagSet(enum TmFlags tmFlagIndex);
/** Returns whether all the quests for the specified tm have been completed */
bool8 TmIsMastered(enum TmPages tmIndex);


/** The move that is currently selected by the tm menu */
enum Move TmCurrentlySelectedMove(void);

void ItemUseOutOfBattle_TechniqueManual(u8 taskId);

void CB2_ReopenTechniqueManual(void);

#endif
