#ifndef GUARD_TECHNIQUE_MANUAL_H
#define GUARD_TECHNIQUE_MANUAL_H

#include "global.h"
#include "constants/technique_manual.h"
#include "constants/technique_manual_flags.h"
#include "constants/technique_manual_pages.h"

/** Increments counters about the currently-being-used battle move */
void TmIncrementSeenStats(u16 move, u16 attackerSpecies);
/** Sets a TM flag */
void TmSetFlag(enum TmFlags tmFlagIndex);
/** Returns whether a TM flag has been set */
bool8 TmIsFlagSet(enum TmFlags tmFlagIndex);
/** Returns whether all the quests for the specified tm have been completed */
bool8 TmIsMastered(enum TmPages tmIndex);


/** The move that is currently selected by the tm menu */
u16 TmCurrentlySelectedMove(void);

void ItemUseOutOfBattle_TechniqueManual(u8 taskId);

void CB2_ReopenTechniqueManual(void);

#endif
